#!/usr/bin/env python3
"""Gymnasium wrapper around :class:`zappy_sim.ZappySim` for stable-baselines3.

Thin on purpose: all the game logic and the obs/action contract live in
``zappy_sim`` (which reuses ``rl_agent``), so this only adapts shapes to the
gymnasium API. One env = one drone training a single level-tier.
"""

from __future__ import annotations

import numpy as np
import gymnasium as gym
from gymnasium import spaces

from zappy_sim import ZappySim, SimConfig, _Reward


class ZappyEnv(gym.Env):
    metadata = {"render_modes": []}

    def __init__(self, start_level=1, width=15, height=15, max_steps=400,
                 require_players=False, preseed_prob=0.0, reward=None, seed=None):
        super().__init__()
        self._make = lambda s: ZappySim(
            SimConfig(width=width, height=height, start_level=start_level,
                      require_players=require_players, max_steps=max_steps,
                      preseed_prob=preseed_prob, seed=s),
            reward or _Reward())
        self._sim = self._make(seed)
        self.observation_space = spaces.Box(
            low=0.0, high=np.inf, shape=(self._sim.obs_size,), dtype=np.float32)
        self.action_space = spaces.Discrete(self._sim.num_actions)

    def reset(self, *, seed=None, options=None):
        super().reset(seed=seed)
        if seed is not None:
            self._sim = self._make(seed)
        obs = self._sim.reset()
        return np.asarray(obs, dtype=np.float32), {}

    def step(self, action):
        obs, reward, done, info = self._sim.step(int(action))
        # terminated = real end (climbed a level = success, or starved = failure);
        # truncated = ran out of decision budget.
        terminated = bool("elevated_to" in info or info.get("reason") == "starved")
        truncated = bool(done and not terminated)
        return np.asarray(obs, dtype=np.float32), float(reward), terminated, truncated, info


# Quick check (needs gymnasium): python3 zappy_env.py
if __name__ == "__main__":
    env = ZappyEnv(start_level=1, seed=0)
    obs, _ = env.reset()
    assert obs.shape == (664,), obs.shape
    import rl_agent
    assert env.action_space.n == len(rl_agent.ACTIONS)
    total = 0.0
    for _ in range(400):
        obs, r, term, trunc, info = env.step(env.action_space.sample())
        total += r
        if term or trunc:
            break
    print(f"env ok: obs{obs.shape} actions={env.action_space.n} ep_return={total:.2f}")
