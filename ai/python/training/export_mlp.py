#!/usr/bin/env python3
"""Export a trained stable-baselines3 PPO policy to the ZAPPY_MLP v1 format that
``ai/python/rl_agent.py`` loads at inference.

The inference loader (``rl_agent.MlpModel``) expects, as plain text:

    ZAPPY_MLP 1
    <input_size> <num_layers>
    <out> <in> <tanh?>      # per layer, then its weights then its biases
    w... (out*in, row-major)
    b... (out)
    ...

and does ``x -> (tanh(Wx+b) if tanh else Wx+b)`` layer by layer, then ``argmax``.

So we export exactly the policy network used to pick the action:

    obs(664) -> [policy_net Linear+Tanh]* -> action_net Linear(18)   # last: no tanh

This MUST match the existing models, which are ``664 -> 64(tanh) -> 64(tanh) ->
18(linear)`` (SB3 ``MlpPolicy`` with ``net_arch=[64, 64]`` and ``Tanh``).

IMPORTANT: train WITHOUT ``VecNormalize`` (or fold it in yourself). rl_agent runs
the raw ``encode_observation`` vector through the net with no normalisation, so a
VecNormalize layer would silently break the policy once deployed.

CLI:  python3 export_mlp.py model.zip ai/models/zappy_ppo_tier2_best.mlp
"""

from __future__ import annotations

import sys
from pathlib import Path


def write_zappy_mlp(path, input_size, layers):
    """Write a ZAPPY_MLP v1 file. ``layers`` = list of (weights2d, bias1d, use_tanh).

    weights2d[r] is the row of ``in_size`` weights for output neuron r (PyTorch
    Linear.weight layout). Pure Python - no third-party dependency - so it can be
    unit-tested against rl_agent's loader without torch.
    """
    out = ["ZAPPY_MLP 1", f"{input_size} {len(layers)}"]
    for weights, bias, use_tanh in layers:
        out_dim = len(weights)
        in_dim = len(weights[0]) if out_dim else 0
        if len(bias) != out_dim:
            raise ValueError(f"bias len {len(bias)} != out_dim {out_dim}")
        out.append(f"{out_dim} {in_dim} {1 if use_tanh else 0}")
        out.append(" ".join(repr(float(w)) for row in weights for w in row))
        out.append(" ".join(repr(float(b)) for b in bias))
    Path(path).write_text("\n".join(out) + "\n")


def export_sb3(model, out_path):
    """Extract the policy MLP from an SB3 PPO model and write it as ZAPPY_MLP.

    ``model`` may be a loaded ``stable_baselines3.PPO`` or a path to its ``.zip``.
    """
    import torch  # noqa: F401  (only needed on the training box)
    from stable_baselines3 import PPO
    import torch.nn as nn

    if isinstance(model, (str, Path)):
        model = PPO.load(str(model), device="cpu")

    policy = model.policy
    layers = []

    # 1) the shared/pi MLP: a Sequential of Linear (+ Tanh) blocks.
    policy_net = policy.mlp_extractor.policy_net
    modules = list(policy_net)
    for i, mod in enumerate(modules):
        if isinstance(mod, nn.Linear):
            use_tanh = i + 1 < len(modules) and isinstance(modules[i + 1], nn.Tanh)
            layers.append((mod.weight.detach().cpu().tolist(),
                           mod.bias.detach().cpu().tolist(), use_tanh))

    # 2) the action head: a final Linear, no activation (argmax over logits).
    action_net = policy.action_net
    if not isinstance(action_net, nn.Linear):
        raise TypeError(f"expected Linear action_net, got {type(action_net)}")
    layers.append((action_net.weight.detach().cpu().tolist(),
                   action_net.bias.detach().cpu().tolist(), False))

    input_size = len(layers[0][0][0])
    write_zappy_mlp(out_path, input_size, layers)
    print(f"exported {out_path}  ({input_size} -> "
          + " -> ".join(str(len(w)) for w, _, _ in layers) + ")")


# --------------------------------------------------------------------------- #
# Round-trip self-test against rl_agent's loader (no torch needed).
#   python3 export_mlp.py --selftest
# --------------------------------------------------------------------------- #
def _selftest():
    sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
    import rl_agent
    import tempfile
    import math

    # A tiny 3->2(tanh)->2(linear) net with known weights.
    layers = [
        ([[0.1, 0.2, 0.3], [-0.1, 0.0, 0.5]], [0.01, -0.02], True),
        ([[1.0, -1.0], [0.5, 0.5]], [0.0, 0.1], False),
    ]
    x = [0.5, -0.5, 1.0]
    # reference forward (same maths as rl_agent.MlpModel.forward)
    h = [math.tanh(sum(w * v for w, v in zip(row, x)) + b) for row, b in zip(layers[0][0], layers[0][1])]
    ref = [sum(w * v for w, v in zip(row, h)) + b for row, b in zip(layers[1][0], layers[1][1])]

    with tempfile.NamedTemporaryFile("w", suffix=".mlp", delete=False) as f:
        path = f.name
    write_zappy_mlp(path, 3, layers)
    model = rl_agent.MlpModel.load(path)
    got = model.forward(x)
    ok = all(abs(a - b) < 1e-9 for a, b in zip(got, ref))
    print("round-trip", "OK" if ok else "FAILED", "ref=", ref, "got=", got)
    if not ok:
        sys.exit(1)


def main():
    if len(sys.argv) == 2 and sys.argv[1] == "--selftest":
        _selftest()
        return
    if len(sys.argv) != 3:
        print("usage: export_mlp.py <sb3_model.zip> <out.mlp>   |   --selftest")
        sys.exit(2)
    export_sb3(sys.argv[1], sys.argv[2])


if __name__ == "__main__":
    main()
