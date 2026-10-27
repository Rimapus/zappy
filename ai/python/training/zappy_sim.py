#!/usr/bin/env python3
"""Single-agent Zappy simulator for training the per-level RL policies.

Why a simulator (and not the real server): PPO needs millions of steps, far more
than a real-time TCP game can provide. So we reproduce the server's mechanics
fast and in-process, and - crucially - reuse ``rl_agent.encode_observation`` and
``rl_agent.ACTIONS`` so the **training observation and action space are byte-for-
byte the inference ones**. A policy trained here drops straight into
``ai/python/rl_agent.py`` once exported (see ``export_mlp.py``).

What is reproduced from the server (read off server/src):
  * toroidal map, resources at the subject densities, refilled every 20 t-units
  * the Look cone layout (row r holds 2r+1 tiles, index r*r is its left edge)
  * food drain (1 unit / 126 t-units), 10 starting food
  * Take/Set, Forward/Left/Right, Look/Inventory/Connect_nbr timings
  * the elevation stone table (Requirements)

Deliberate simplification: this is SINGLE agent, so the multi-player quorum of a
ritual cannot be met for real. For training we grant the elevation as soon as the
**stones** for the current level are on the tile (the policy learns its own job -
gather, position, incant - and the real multi-drone deployment supplies the
team-mates). Tune ``require_players`` to False/True to change that.

No third-party dependency: pure Python, usable for a fast standalone smoke test.
The gymnasium wrapper lives in ``zappy_env.py``.
"""

from __future__ import annotations

import random
import sys
from dataclasses import dataclass, field
from pathlib import Path

# Reuse the *inference* contract so training can never drift from it.
sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
import rl_agent  # noqa: E402  (ACTIONS, RESOURCES, encode_observation, requirement table)

# Stones in canonical server order (Requirements.cpp / Look RESOURCES order).
STONES = ["linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"]

# resource -> spawn density (subject §2). Food included.
DENSITY = {
    "food": 0.5,
    "linemate": 0.3,
    "deraumere": 0.15,
    "sibur": 0.1,
    "mendiane": 0.1,
    "phiras": 0.08,
    "thystame": 0.05,
}

# Elevation requirements [level-1] -> (players, {stone: count}). Mirrors the
# server's ELEVATION_REQUIREMENTS and the AI's Requirements.cpp.
ELEVATION = [
    (1, {"linemate": 1}),
    (2, {"linemate": 1, "deraumere": 1, "sibur": 1}),
    (2, {"linemate": 2, "sibur": 1}),
    (4, {"linemate": 1, "deraumere": 1, "sibur": 2, "phiras": 1}),
    (4, {"linemate": 1, "deraumere": 2, "sibur": 1, "mendiane": 3}),
    (6, {"linemate": 1, "deraumere": 2, "sibur": 3, "phiras": 1}),
    (6, {"linemate": 2, "deraumere": 2, "sibur": 2, "mendiane": 2, "phiras": 2, "thystame": 1}),
]

# Direction unit vectors, indexed 0=N,1=E,2=S,3=W (server FORWARD_OFFSETS order).
FORWARD = [(0, -1), (1, 0), (0, 1), (-1, 0)]
LEFT = [(-1, 0), (0, -1), (1, 0), (0, 1)]

FOOD_START = 10
FOOD_INTERVAL = 126          # one food unit burned per this many time units
RESPAWN_INTERVAL = 20        # natural resources refilled to target this often
MAX_LEVEL = 8

# Action durations in time units (the server's "action / f", with f folded out -
# we work in raw time units; 'f' only rescales wall-clock, not the policy).
ACTION_TIME = {
    "Forward": 7, "Right": 7, "Left": 7, "Look": 7, "Inventory": 1,
    "Connect_nbr": 0, "Fork": 42, "Eject": 7, "Incantation": 300, "Set food": 7,
}
for _s in STONES:
    ACTION_TIME[f"Take {_s}"] = 7
    ACTION_TIME[f"Set {_s}"] = 7
ACTION_TIME["Take food"] = 7


@dataclass
class SimConfig:
    width: int = 15
    height: int = 15
    start_level: int = 1            # tier under training: drone spawns at this level
    require_players: bool = False   # single-agent: usually grant on stones alone
    max_steps: int = 400            # episode cap (decisions, not time units)
    done_on_elevate: bool = True    # end the episode after climbing one level (per-tier task)
    preseed_prob: float = 0.0       # curriculum: chance to start already CARRYING the
                                    # needed stones, so the policy first learns the short
                                    # Set->Incantation tail before the long gather chain.
    seed: int | None = None


@dataclass
class _Reward:
    elevate: float = 15.0           # the whole point: climb a level (big payoff)
    gather_needed: float = 0.3      # took a still-needed stone (small, capped - no farming)
    drop_needed: float = 2.0        # set a needed stone onto the altar tile (the key step
                                    # the old reward never taught - so make it count)
    eat_low: float = 0.6            # ate while food was getting low (survival)
    death: float = -15.0            # starved
    step: float = -0.01             # mild pressure to be efficient
    wasted: float = -0.05           # a ko / pointless action


class ZappySim:
    """One drone, one level-tier, the game's mechanics - fast and dependency-free."""

    def __init__(self, config: SimConfig | None = None, reward: _Reward | None = None):
        self._cfg = config or SimConfig()
        self._r = reward or _Reward()
        self._rng = random.Random(self._cfg.seed)
        self.action_names = list(rl_agent.ACTIONS)   # the 18 inference actions, same order
        self.obs_size = rl_agent.OBS_SIZE            # 664
        self.num_actions = len(self.action_names)    # 18
        self._reset_state()

    # ---- lifecycle -------------------------------------------------------- #
    def reset(self):
        self._reset_state()
        self._spawn_resources(initial=True)
        return self._encode()

    def _reset_state(self):
        w, h = self._cfg.width, self._cfg.height
        self._tiles = [{k: 0 for k in DENSITY} for _ in range(w * h)]
        self._x = self._rng.randrange(w)
        self._y = self._rng.randrange(h)
        self._dir = self._rng.randrange(4)
        self._level = self._cfg.start_level
        self._inv = {k: 0 for k in STONES}
        self._inv["food"] = FOOD_START
        # Curriculum: sometimes hand the drone the stones it needs, so it can learn
        # the short Set->Incantation tail without first solving the long search.
        if self._cfg.preseed_prob and self._rng.random() < self._cfg.preseed_prob:
            for stone, count in self._need().items():
                self._inv[stone] = count
        # Per-episode caps so gather/drop reward can't be farmed in a take<->set
        # loop: each is paid at most `required` times for a given stone.
        self._take_paid = {k: 0 for k in STONES}
        self._set_paid = {k: 0 for k in STONES}
        self._clock = 0
        self._next_food = FOOD_INTERVAL
        self._next_respawn = RESPAWN_INTERVAL
        self._steps = 0
        self._dead = False

    # ---- map helpers ------------------------------------------------------ #
    def _idx(self, x, y):
        return (y % self._cfg.height) * self._cfg.width + (x % self._cfg.width)

    def _tile(self, x, y):
        return self._tiles[self._idx(x, y)]

    def _natural_count(self, res):
        return sum(t[res] for t in self._tiles)

    def _spawn_resources(self, initial=False):
        area = self._cfg.width * self._cfg.height
        for res, dens in DENSITY.items():
            target = max(1, int(area * dens))
            current = self._natural_count(res)
            for _ in range(max(0, target - current)):
                self._tile(self._rng.randrange(self._cfg.width),
                           self._rng.randrange(self._cfg.height))[res] += 1

    # ---- observation (reuses the inference encoder) ----------------------- #
    def _vision_tiles(self):
        """List of tiles (each a list of object name strings) in server cone order."""
        fwd = FORWARD[self._dir]
        lft = LEFT[self._dir]
        rgt = (-lft[0], -lft[1])
        out = []
        for line in range(self._level + 1):
            row_x = self._x + (fwd[0] + lft[0]) * line
            row_y = self._y + (fwd[1] + lft[1]) * line
            for i in range(2 * line + 1):
                tx, ty = row_x + rgt[0] * i, row_y + rgt[1] * i
                names = []
                if line == 0 and i == 0:
                    names.append("player")            # only self, and only on tile 0
                tile = self._tile(tx, ty)
                for res in DENSITY:
                    names.extend([res] * tile[res])
                out.append(names)
        return out

    def _obs_dict(self):
        inv = {"food": self._inv["food"]}
        for s in STONES:
            inv[s] = self._inv[s]
        return {"level": self._level, "inventory": inv,
                "vision": [[n for n in tile] for tile in self._vision_tiles()]}

    def _encode(self):
        # encode_observation expects tiles exposing .count(name); give it objects.
        obs = self._obs_dict()
        obs["vision"] = [_Tile(names) for names in obs["vision"]]
        return rl_agent.encode_observation(obs)

    # ---- time / survival -------------------------------------------------- #
    def _advance_time(self, dt):
        self._clock += dt
        ate_penalty = False
        while self._clock >= self._next_food:
            self._next_food += FOOD_INTERVAL
            if self._inv["food"] <= 0:
                self._dead = True
                return ate_penalty
            self._inv["food"] -= 1
        while self._clock >= self._next_respawn:
            self._next_respawn += RESPAWN_INTERVAL
            self._spawn_resources()
        return ate_penalty

    # ---- step ------------------------------------------------------------- #
    def step(self, action_index):
        if self._dead:
            return self._encode(), 0.0, True, {"reason": "already_dead"}
        self._steps += 1
        command = self.action_names[action_index % self.num_actions]
        reward = self._r.step
        info = {}

        reward += self._apply(command, info)
        self._advance_time(ACTION_TIME.get(command, 7))

        elevated = "elevated_to" in info
        done = (self._dead or self._steps >= self._cfg.max_steps
                or self._level > MAX_LEVEL
                or (elevated and self._cfg.done_on_elevate))
        if self._dead:
            reward += self._r.death
            info["reason"] = "starved"
        return self._encode(), reward, done, info

    def _apply(self, command, info):
        if command == "Forward":
            dx, dy = FORWARD[self._dir]
            self._x = (self._x + dx) % self._cfg.width
            self._y = (self._y + dy) % self._cfg.height
            return 0.0
        if command == "Right":
            self._dir = (self._dir + 1) % 4
            return 0.0
        if command == "Left":
            self._dir = (self._dir + 3) % 4
            return 0.0
        if command in ("Look", "Inventory", "Connect_nbr", "Fork", "Eject") \
                or command.startswith("Broadcast"):
            return 0.0                                   # legal no-ops for one drone
        if command.startswith("Take "):
            return self._take(command[5:], info)
        if command.startswith("Set "):
            return self._set(command[4:], info)
        if command == "Incantation":
            return self._incant(info)
        return self._r.wasted

    def _need(self):
        return ELEVATION[self._level - 1][1] if 1 <= self._level <= 7 else {}

    def _take(self, res, info):
        tile = self._tile(self._x, self._y)
        if res == "food":
            if tile["food"] > 0:
                tile["food"] -= 1
                low = self._inv["food"] < 8
                self._inv["food"] += 1
                return self._r.eat_low if low else 0.0
            return self._r.wasted
        if res in STONES and tile[res] > 0:
            tile[res] -= 1
            self._inv[res] += 1
            required = self._need().get(res, 0)
            if self._take_paid[res] < required:
                self._take_paid[res] += 1
                return self._r.gather_needed
            return 0.0
        return self._r.wasted

    def _set(self, res, info):
        tile = self._tile(self._x, self._y)
        if res == "food" and self._inv["food"] > 0:
            self._inv["food"] -= 1
            tile["food"] += 1
            return 0.0
        if res in STONES and self._inv[res] > 0:
            self._inv[res] -= 1
            tile[res] += 1
            # the altar-building step the old reward never taught - paid at most
            # `required` times per episode so a take<->set loop earns nothing extra.
            required = self._need().get(res, 0)
            if self._set_paid[res] < required:
                self._set_paid[res] += 1
                return self._r.drop_needed
            return 0.0
        return self._r.wasted

    def _incant(self, info):
        if not 1 <= self._level <= 7:
            return self._r.wasted
        players, stones = ELEVATION[self._level - 1]
        tile = self._tile(self._x, self._y)
        if any(tile[s] < n for s, n in stones.items()):
            return self._r.wasted                       # missing stones -> ko
        if self._cfg.require_players and players > 1:
            return self._r.wasted                       # quorum unmet (single agent)
        for s, n in stones.items():
            tile[s] -= n
        self._level += 1
        info["elevated_to"] = self._level
        return self._r.elevate


class _Tile:
    """Minimal stand-in exposing .count() as rl_agent.encode_observation expects."""

    __slots__ = ("_names",)

    def __init__(self, names):
        self._names = names

    def count(self, name):
        return self._names.count(name)


# --------------------------------------------------------------------------- #
# Standalone smoke test (no third-party deps): python3 zappy_sim.py
# --------------------------------------------------------------------------- #
def _smoke():
    rng = random.Random(0)
    sim = ZappySim(SimConfig(width=12, height=12, start_level=1, seed=1))
    obs = sim.reset()
    assert len(obs) == sim.obs_size == 664, f"obs size {len(obs)} != 664"
    assert sim.num_actions == len(rl_agent.ACTIONS), sim.num_actions
    elevations = 0
    deaths = 0
    for episode in range(200):
        obs = sim.reset()
        total = 0.0
        for _ in range(sim._cfg.max_steps):
            obs, reward, done, info = sim.step(rng.randrange(sim.num_actions))
            assert len(obs) == 664
            total += reward
            if "elevated_to" in info:
                elevations += 1
            if done:
                if info.get("reason") == "starved":
                    deaths += 1
                break
    print(f"smoke ok: obs={sim.obs_size} actions={sim.num_actions} "
          f"random-policy elevations={elevations} deaths={deaths} over 200 eps")
    # A heuristic gather-then-incant rollout should elevate from L1 easily.
    sim = ZappySim(SimConfig(width=10, height=10, start_level=1, seed=2))
    sim.reset()
    got = False
    for _ in range(800):
        # cheat policy: stand still, set a carried linemate, else take linemate, else look
        cmd = "Incantation"
        if sim._tile(sim._x, sim._y)["linemate"] < 1:
            cmd = "Set linemate" if sim._inv["linemate"] > 0 else "Take linemate"
            if sim._inv["linemate"] == 0 and sim._tile(sim._x, sim._y)["linemate"] == 0:
                cmd = "Forward"
        _, _, done, info = sim.step(sim.action_names.index(cmd))
        if "elevated_to" in info:
            got = True
            print(f"heuristic reached level {info['elevated_to']} - mechanics work")
            break
        if done:
            sim.reset()
    if not got:
        print("heuristic did not elevate (check mechanics)")


if __name__ == "__main__":
    _smoke()
