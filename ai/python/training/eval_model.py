#!/usr/bin/env python3
"""Score trained Zappy tier policies WITHOUT the server or torch.

Loads each exported ``.mlp`` with rl_agent's own pure-Python loader and runs it
greedily (argmax, exactly like inference) in the simulator, then reports, per
tier, how often it climbs the level and how often it starves. This is the first,
fast "is the model any good?" check; the real-server smoke test (below) is the
ground truth for sim->real transfer.

    python3 eval_model.py                       # all of tiers 2..5, 300 eps each
    python3 eval_model.py --tiers 2 3 4 --episodes 500
    python3 eval_model.py --models-dir ../../models
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(HERE.parent))            # rl_agent
import rl_agent                                  # noqa: E402
from zappy_sim import ZappySim, SimConfig        # noqa: E402

# The level(s) each tier actually serves at inference (level_to_tier = clamp 2..7):
# tier2 covers BOTH level 1 (trivial, 1 stone) and level 2 (L2->3, 3 stones), so
# test both - a tier2 that aces L1 can still flunk L2.
TIER_LEVELS = {2: [1, 2], 3: [3], 4: [4], 5: [5], 6: [6], 7: [7]}
# Per-level episode cap (more stones -> longer), mirroring train.py.
LEVEL_MAX_STEPS = {1: 250, 2: 450, 3: 500, 4: 600, 5: 700, 6: 800, 7: 900}


def make_forward(model):
    """Return a fast forward(x)->scores. Uses numpy if present (VM), else rl_agent's
    pure-Python forward (works anywhere, just slower)."""
    try:
        import numpy as np
    except ImportError:
        return model.forward
    weights = [np.asarray(w, dtype=np.float64) for w, _, _ in model._layers]
    biases = [np.asarray(b, dtype=np.float64) for _, b, _ in model._layers]
    tanhs = [t for _, _, t in model._layers]

    def fwd(x):
        v = np.asarray(x, dtype=np.float64)
        for w, b, t in zip(weights, biases, tanhs):
            v = w @ v + b
            if t:
                v = np.tanh(v)
        return v
    return fwd


def evaluate(model, level, episodes, max_steps, width, height, seed):
    sim = ZappySim(SimConfig(width=width, height=height, start_level=level,
                             max_steps=max_steps, done_on_elevate=True, seed=seed))
    forward = make_forward(model)
    elevated = deaths = 0
    steps_to_win = []
    for ep in range(episodes):
        obs = sim.reset()
        for t in range(max_steps):
            scores = forward(obs)
            action = max(range(len(scores)), key=lambda i: scores[i]) % sim.num_actions
            obs, _, done, info = sim.step(action)
            if "elevated_to" in info:
                elevated += 1
                steps_to_win.append(t + 1)
                break
            if done:
                if info.get("reason") == "starved":
                    deaths += 1
                break
    win = 100.0 * elevated / episodes
    die = 100.0 * deaths / episodes
    mean_steps = sum(steps_to_win) / len(steps_to_win) if steps_to_win else 0.0
    return win, die, mean_steps


def main():
    ap = argparse.ArgumentParser(description="Score trained tier policies in the sim.")
    ap.add_argument("--tiers", type=int, nargs="+", default=[2, 3, 4, 5])
    ap.add_argument("--episodes", type=int, default=200,
                    help="more = tighter estimate. Fast with numpy (on the VM).")
    ap.add_argument("--max-steps", type=int, default=0,
                    help="0 = per-tier default (higher tiers get a longer episode).")
    ap.add_argument("--width", type=int, default=15)
    ap.add_argument("--height", type=int, default=15)
    ap.add_argument("--seed", type=int, default=12345)
    ap.add_argument("--models-dir", default=str(HERE.parent.parent / "models"))
    args = ap.parse_args()

    models_dir = Path(args.models_dir)
    print(f"models: {models_dir}\n")
    print(f"{'tier':>4} {'level':>5} {'elevate%':>9} {'death%':>7} {'avg_steps':>10}  verdict")
    print("-" * 56)
    for tier in args.tiers:
        for level in TIER_LEVELS.get(tier, [tier]):
            path = rl_agent.model_path(models_dir, level)
            if path is None:
                print(f"{tier:>4} {level:>5}   (no model file found)")
                continue
            try:
                model = rl_agent.MlpModel.load(path)
            except (OSError, ValueError) as err:
                print(f"{tier:>4} {level:>5}   FAILED to load: {err}")
                continue
            steps_cap = args.max_steps or LEVEL_MAX_STEPS.get(level, 500)
            win, die, steps = evaluate(model, level, args.episodes, steps_cap,
                                       args.width, args.height, args.seed + level)
            verdict = "good" if win >= 80 and die <= 10 else \
                      "weak" if win >= 40 else "broken/undertrained"
            print(f"{tier:>4} {level:>5} {win:>8.1f}% {die:>6.1f}% {steps:>10.1f}  {verdict}")

    print("\nReference points (in this sim):")
    print("  random policy ~30-40% elevate, ~50-70% death.")
    print("  a well-trained tier should be >=80% elevate and <=10% death.")
    print("\nGround truth (sim->real transfer) - run the real server:")
    print("  cmake --build ai/build -j && ./ai/tests/test_model.sh")
    print("  watch it:  MODE=model ./ai/demo/run_gui.sh")


if __name__ == "__main__":
    main()
