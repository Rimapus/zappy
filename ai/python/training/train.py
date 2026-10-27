#!/usr/bin/env python3
"""Train the per-tier Zappy PPO policies and export them to ZAPPY_MLP for
``ai/python/rl_agent.py``.

One policy per tier (``zappy_ppo_tier{N}_best.mlp``). The inference side maps a
player level to a tier with ``clamp(level, 2, 7)`` (see rl_agent.level_to_tier),
so:  tier2 covers levels 1-2,  tier3 -> 3,  tier4 -> 4,  tier5 -> 5,  ...
"The first 4 levels by AI" therefore means training tiers **2, 3, 4** (and 5 if
you want the 4->5 elevation handled too).

Architecture is fixed to match the existing models and the exporter:
``MlpPolicy`` with ``net_arch=[64, 64]`` and ``Tanh`` -> exported as
``664 -> 64(tanh) -> 64(tanh) -> 18(linear)``. Do NOT add VecNormalize (rl_agent
feeds the raw encode_observation vector with no normalisation).

Run on your VM (35 cores / 70 GB) - examples at the bottom of the file:

    python3 train.py --tiers 2 3 4 5 --envs 32 --steps 8000000

Needs: stable-baselines3, gymnasium, torch (CPU is fine for a [64,64] net).
"""

from __future__ import annotations

import argparse
import os
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))                 # zappy_env, zappy_sim, export_mlp
sys.path.insert(0, str(HERE.parent))          # rl_agent

from zappy_sim import _Reward                  # noqa: E402
from export_mlp import export_sb3              # noqa: E402


# Higher tiers need a longer episode (more stones to assemble).
TIER_MAX_STEPS = {2: 450, 3: 500, 4: 600, 5: 700, 6: 800, 7: 900}
# Curriculum OFF by default: a *fixed* preseed (start carrying the stones) made
# the policy rely on being handed them and then flunk the real, empty-handed
# task at deploy/eval (it knocked tier2 from 100% to 0%). Only opt in if you also
# anneal it to 0 over training.
TIER_PRESEED = {}


def make_env_factory(tier, width, height, max_steps):
    """Return a thunk building one ZappyEnv for the given tier.

    tier2 also covers level 1, so it trains on a mix of start levels 1 and 2.
    ``max_steps`` of 0 means "use the per-tier default".
    """
    import random
    from zappy_env import ZappyEnv

    steps = max_steps or TIER_MAX_STEPS.get(tier, 500)
    preseed = TIER_PRESEED.get(tier, 0.0)

    def _thunk():
        # SubprocVecEnv calls this in a child process.
        start = tier
        if tier == 2:
            start = random.choice([1, 2])
        return ZappyEnv(start_level=start, width=width, height=height,
                        max_steps=steps, require_players=False,
                        preseed_prob=preseed, reward=_Reward())
    return _thunk


def _progress_bar_available():
    """SB3's progress bar needs both tqdm and rich; fall back gracefully if not."""
    try:
        import tqdm  # noqa: F401
        import rich  # noqa: F401
        return True
    except ImportError:
        return False


def train_tier(tier, args, models_dir):
    from stable_baselines3 import PPO
    from stable_baselines3.common.vec_env import SubprocVecEnv
    from stable_baselines3.common.monitor import Monitor
    import torch.nn as nn

    def monitored():
        env = make_env_factory(tier, args.width, args.height, args.max_steps)()
        return Monitor(env)

    vec = SubprocVecEnv([monitored for _ in range(args.envs)], start_method="spawn")

    policy_kwargs = dict(net_arch=[64, 64], activation_fn=nn.Tanh)
    model = PPO(
        "MlpPolicy", vec,
        n_steps=args.n_steps, batch_size=args.batch_size, n_epochs=args.n_epochs,
        gamma=0.99, gae_lambda=0.95, ent_coef=args.ent_coef, learning_rate=args.lr,
        clip_range=0.2, policy_kwargs=policy_kwargs, verbose=1, device="cpu",
        seed=args.seed + tier,
    )
    use_bar = not args.no_progress and _progress_bar_available()
    if not use_bar and not args.no_progress:
        print("(no tqdm/rich -> progress bar off; `pip install tqdm rich` to enable)")
    print(f"\n=== training tier {tier} : {args.steps:,} steps on {args.envs} envs ===")
    model.learn(total_timesteps=args.steps, progress_bar=use_bar)

    zip_path = models_dir / f"zappy_ppo_tier{tier}.zip"
    model.save(str(zip_path))
    out = models_dir / f"zappy_ppo_tier{tier}_best.mlp"
    export_sb3(model, str(out))
    vec.close()
    print(f"=== tier {tier} done -> {out} ===")


def main():
    ap = argparse.ArgumentParser(description="Train Zappy per-tier PPO policies.")
    ap.add_argument("--tiers", type=int, nargs="+", default=[2, 3, 4, 5],
                    help="tiers to train (2..7). 'first 4 levels' = 2 3 4 (+5).")
    ap.add_argument("--envs", type=int, default=min(32, (os.cpu_count() or 8) - 2),
                    help="parallel environments (leave a couple cores for the OS).")
    ap.add_argument("--steps", type=int, default=8_000_000, help="timesteps per tier.")
    ap.add_argument("--n-steps", type=int, default=2048, help="rollout length per env.")
    ap.add_argument("--batch-size", type=int, default=8192)
    ap.add_argument("--n-epochs", type=int, default=10)
    ap.add_argument("--lr", type=float, default=3e-4)
    ap.add_argument("--ent-coef", type=float, default=0.02,
                    help="entropy bonus; higher = more exploration (helps discover "
                         "the Take->Set->Incantation sequence).")
    ap.add_argument("--width", type=int, default=15)
    ap.add_argument("--height", type=int, default=15)
    ap.add_argument("--max-steps", type=int, default=0,
                    help="episode decision cap; 0 = per-tier default (longer for "
                         "higher tiers, which must assemble more stones).")
    ap.add_argument("--seed", type=int, default=0)
    ap.add_argument("--no-progress", action="store_true",
                    help="disable the tqdm/rich progress bar (SB3 still logs stats).")
    ap.add_argument("--models-dir", default=str(HERE.parent.parent / "models"),
                    help="where to write zappy_ppo_tier{N}_best.mlp (default ai/models).")
    args = ap.parse_args()

    models_dir = Path(args.models_dir)
    models_dir.mkdir(parents=True, exist_ok=True)
    for tier in args.tiers:
        if not 2 <= tier <= 7:
            print(f"skip tier {tier} (inference only uses tiers 2..7)")
            continue
        train_tier(tier, args, models_dir)

    print("\nAll done. Validate against the real server:")
    print("  cmake --build ai/build -j && ./ai/demo/run_gui.sh MODE=model")
    print("  # or headless: python3 ai/tests/monitor.py <port> <secs> while model drones run")


if __name__ == "__main__":
    main()
