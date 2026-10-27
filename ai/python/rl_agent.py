#!/usr/bin/env python3
"""Per-level RL agent for the Zappy AI.

The C++ client (see ``ModelPolicy`` / ``RlBridge``) speaks to this process over
stdin/stdout, one line each way:

    stdin  <- one JSON observation per line (see ``Serializer`` on the C++ side)
    stdout -> one protocol command per line ("Forward", "Take linemate", ...)

"One model per level": the observation carries the player's ``level`` and we run
the model trained for that level. Models live in the directory passed as argv[1]
(default ``ai/models``); the per-level file is resolved by ``model_path``.

The C++ side stays free of any ML dependency - all the maths is here, and this
module has **no third-party dependency** (pure-Python forward pass) so it runs
anywhere Python 3 does.

Drop-in points for the training side (kept explicit on purpose):
  * ``ACTIONS``           - must match the action space the models were trained on.
  * ``encode_observation``- must match the training observation encoder.
  * ``model_path``        - must match how the trained models are named on disk.
"""

import json
import math
import sys
from pathlib import Path

# --------------------------------------------------------------------------- #
# Action space - index -> protocol command. MUST match the training setup.
# --------------------------------------------------------------------------- #
ACTIONS = [
    "Forward",
    "Right",
    "Left",
    "Look",
    "Inventory",
    "Connect_nbr",
    "Fork",
    "Eject",
    "Broadcast level",          # rally on own level; refined below with the real level
    "Incantation",
    "Take food",
    "Take linemate",
    "Take deraumere",
    "Take sibur",
    "Take mendiane",
    "Take phiras",
    "Take thystame",
    "Set food",
    # Stone drops, appended after the original 18 so existing indices are
    # preserved. These let a drone lay stones on its tile to build the altar -
    # without them the elevation prerequisite can never be assembled.
    "Set linemate",
    "Set deraumere",
    "Set sibur",
    "Set mendiane",
    "Set phiras",
    "Set thystame",
]

# Resource channel order for a vision tile / the inventory.
RESOURCES = ["food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"]

MAX_LEVEL = 8
# A level-L Look cone holds (L+1)^2 tiles; the model input is sized for level 8.
MAX_TILES = (MAX_LEVEL + 1) ** 2          # 81
TILE_CHANNELS = len(RESOURCES) + 1        # 7 resources + visible-player count = 8
OBS_SIZE = MAX_TILES * TILE_CHANNELS + len(RESOURCES) + 1 + MAX_LEVEL  # 664

# Safe default when no model is available for the level: keep foraging/alive.
FALLBACK_COMMAND = "Forward"


# --------------------------------------------------------------------------- #
# ZAPPY_MLP model - a tiny dependency-free MLP loaded from the exported format:
#
#   ZAPPY_MLP 1
#   <input_size> <num_layers>
#   <out> <in> <tanh?>     # per layer: dimensions + activation flag (1 = tanh)
#   w0 w1 ... (out*in)      # row-major weights
#   b0 b1 ... (out)         # biases
#   ...                     # repeated for each layer
# --------------------------------------------------------------------------- #
class MlpModel:
    def __init__(self, layers):
        # layers: list of (weights[out][in], bias[out], use_tanh)
        self._layers = layers

    @classmethod
    def load(cls, path):
        tokens = Path(path).read_text().split()
        pos = 0

        def take(n):
            nonlocal pos
            chunk = tokens[pos:pos + n]
            pos += n
            return chunk

        if take(2) != ["ZAPPY_MLP", "1"]:
            raise ValueError(f"{path}: not a ZAPPY_MLP v1 file")
        _input_size = int(take(1)[0])
        num_layers = int(take(1)[0])

        layers = []
        for _ in range(num_layers):
            out_dim, in_dim, use_tanh = (int(x) for x in take(3))
            flat_w = [float(x) for x in take(out_dim * in_dim)]
            weights = [flat_w[r * in_dim:(r + 1) * in_dim] for r in range(out_dim)]
            bias = [float(x) for x in take(out_dim)]
            layers.append((weights, bias, bool(use_tanh)))
        return cls(layers)

    def forward(self, x):
        for weights, bias, use_tanh in self._layers:
            out = [sum(w * v for w, v in zip(row, x)) + b for row, b in zip(weights, bias)]
            x = [math.tanh(v) for v in out] if use_tanh else out
        return x


# --------------------------------------------------------------------------- #
# Observation encoding - JSON observation -> flat float vector for the model.
# MUST mirror the encoder the models were trained with.
# --------------------------------------------------------------------------- #
def encode_observation(obs):
    vector = [0.0] * OBS_SIZE

    # Vision: each tile -> [resource counts..., visible-player count], in cone order.
    vision = obs.get("vision", [])
    for tile_index, tile in enumerate(vision[:MAX_TILES]):
        base = tile_index * TILE_CHANNELS
        for channel, resource in enumerate(RESOURCES):
            vector[base + channel] = float(tile.count(resource))
        vector[base + len(RESOURCES)] = float(tile.count("player"))

    # Inventory: the seven resources the player carries.
    inventory = obs.get("inventory", {})
    inv_base = MAX_TILES * TILE_CHANNELS
    for channel, resource in enumerate(RESOURCES):
        vector[inv_base + channel] = float(inventory.get(resource, 0))

    # Life proxy: remaining food (each unit ~= 126 time units). Normalised.
    life_index = inv_base + len(RESOURCES)
    vector[life_index] = min(float(inventory.get("food", 0)) / 10.0, 1.0)

    # Level one-hot.
    level = int(obs.get("level", 1))
    level_base = life_index + 1
    if 1 <= level <= MAX_LEVEL:
        vector[level_base + level - 1] = 1.0

    return vector


# --------------------------------------------------------------------------- #
# Per-level model resolution & cache.
# --------------------------------------------------------------------------- #
def level_to_tier(level):
    """Map a player level to the trained tier (tier2..tier7).

    The trained set has one model per tier; tier2 also covers level 1 and tier7
    also covers level 8 (same clamp as the training/eval policy server).
    """
    return min(max(level, 2), 7)


def model_path(models_dir, level):
    """Return the model file to use for ``level``, or None.

    Resolves the tier with ``level_to_tier`` then tries the known file names,
    real exported names first.
    """
    tier = level_to_tier(level)
    candidates = [
        models_dir / f"zappy_ppo_tier{tier}_best.mlp",
        models_dir / f"zappy_ppo_tier{tier}.mlp",
        models_dir / f"tier{tier}.mlp",
        models_dir / f"level_{level}.mlp",
        models_dir / "policy.mlp",
    ]
    for candidate in candidates:
        if candidate.is_file():
            return candidate
    return None


class ModelCache:
    def __init__(self, models_dir):
        self._models_dir = Path(models_dir)
        self._cache = {}

    def for_level(self, level):
        tier = level_to_tier(level)
        if tier in self._cache:
            return self._cache[tier]
        model = None
        path = model_path(self._models_dir, level)
        if path is not None:
            try:
                model = MlpModel.load(path)
            except (OSError, ValueError) as error:
                print(f"rl_agent: failed to load {path}: {error}", file=sys.stderr)
        self._cache[tier] = model
        return model


def choose_command(model, obs):
    """Run the model and turn its action index into a protocol command."""
    scores = model.forward(encode_observation(obs))
    action = max(range(len(scores)), key=scores.__getitem__) % len(ACTIONS)
    command = ACTIONS[action]
    if command == "Broadcast level":
        command = f"Broadcast {int(obs.get('level', 1))}"
    return command


def main():
    models_dir = sys.argv[1] if len(sys.argv) > 1 else "ai/models"
    cache = ModelCache(models_dir)

    for line in sys.stdin:
        line = line.strip()
        if not line:
            continue
        try:
            obs = json.loads(line)
        except json.JSONDecodeError:
            print(FALLBACK_COMMAND, flush=True)
            continue

        model = cache.for_level(int(obs.get("level", 1)))
        command = choose_command(model, obs) if model is not None else FALLBACK_COMMAND
        print(command, flush=True)


if __name__ == "__main__":
    main()
