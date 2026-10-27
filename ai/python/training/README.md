# Training the per-tier RL policies (the "real AI" bonus)

`ai/python/rl_agent.py` is **inference only**: it loads one `.mlp` per tier and
runs a pure-Python forward pass. There was **no training code in the repo** — this
folder is a complete, ready-to-run reference pipeline that produces `.mlp` files
`rl_agent.py` loads **as-is**. Use it directly, or check your own script against
the contract below (that is what actually makes or breaks a trained model).

```
zappy_sim.py    fast single-agent Zappy simulator (pure Python, reuses rl_agent)
zappy_env.py    gymnasium wrapper around the sim
train.py        stable-baselines3 PPO, one policy per tier, parallel envs
export_mlp.py   SB3 policy  ->  ZAPPY_MLP v1   (the format rl_agent loads)
requirements.txt
```

## The contract — check ANY training script against these 4 things

A model that gets these wrong still *loads*, then plays randomly. They are all
pinned by `rl_agent.py`; `zappy_sim`/`export_mlp` reuse it so they cannot drift.

1. **Action space** — exactly `rl_agent.ACTIONS`, **24 entries**: the original 18
   (`Forward, Right, Left, Look, Inventory, Connect_nbr, Fork, Eject,
   Broadcast level, Incantation, Take food, Take linemate … Take thystame,
   Set food`) **plus `Set linemate … Set thystame`** appended at the end. Those
   stone-drops are what let a drone build the elevation altar — without them a
   policy can never assemble an incantation's stones and tops out around L1.
   The model outputs 24 logits; `argmax % 24` picks the action. (Old 18-output
   models still load but can't use the new actions — **retrain**.)

2. **Observation** — exactly `rl_agent.encode_observation`, a **664-float** vector:
   `81 tiles × 8 channels` (7 resource counts + visible-player count, in Look-cone
   order) `+ 7 inventory counts + food/10 (clamped to 1) + 8-way level one-hot`.
   Train on this very vector. (The sim builds the JSON obs and calls
   `encode_observation`, so it is guaranteed identical to deployment.)

3. **Model file** — `ZAPPY_MLP v1` text format, and the existing nets are
   `664 -> 64(tanh) -> 64(tanh) -> 24(linear)` (24 = the action count) = SB3 `MlpPolicy`,
   `net_arch=[64,64]`, `activation_fn=Tanh`. `export_mlp.py` writes exactly this;
   `python3 export_mlp.py --selftest` round-trips it through rl_agent's loader.
   **Do not use `VecNormalize`** — rl_agent feeds the raw vector, unnormalised.

4. **File names / tiers** — `zappy_ppo_tier{N}_best.mlp` in `ai/models/`.
   Inference maps level→tier with `clamp(level, 2, 7)`:
   `tier2` serves **levels 1-2**, `tier3`→3, `tier4`→4, … `tier7`→7-8.
   ⚠️ `tier1` is **never loaded** (the clamp floor is 2) — training it is wasted.
   So **"the first 4 levels by AI" = train tiers 2, 3, 4** (add `5` to also own the
   4→5 elevation).

## Run it on your VM (35 cores / 70 GB)

```bash
cd ai/python/training
python3 -m venv .venv && . .venv/bin/activate
pip install -r requirements.txt

# sanity (no training): mechanics + obs/action shapes + export format
python3 zappy_sim.py
python3 export_mlp.py --selftest

# train the first levels' tiers, 32 parallel envs, ~8M steps each
python3 train.py --tiers 2 3 4 5 --envs 32 --steps 8000000
# -> writes ai/models/zappy_ppo_tier{2,3,4,5}_best.mlp  (rl_agent picks them up)
```

Sizing for 35 cores: PPO is CPU-bound here (tiny net), so throughput scales with
`--envs`. Use ~32 (leave 2-3 cores for the main process / OS); 70 GB RAM is far
more than enough (each env is a few MB). One tier at 8M steps is roughly an hour
of wall-clock on that box; the four run sequentially by default. Want them in
parallel instead? Launch four `train.py --tiers N --envs 8` in separate shells.

Knobs that matter: `--steps` (more = better, diminishing after ~10M for the low
tiers), `--max-steps` (decision budget per episode; 400 is plenty for L1-L4),
`--width/--height` (15×15 gives a comfortable resource supply). Reward shaping
lives in `zappy_sim._Reward` (elevate is the big signal; gather/drop/eat are
nudges; death is penalised) — tweak there, not in the contract.

## Validate the trained models against the *real* server

The sim is single-agent, so it grants an elevation on **stones present** alone
(a lone drone can't muster the multi-player quorum). That is fine for teaching
gather/position/incant on the low tiers, but always confirm on the real game:

```bash
cmake --build ai/build -j
# watch model drones in the GUI:
MODE=model ./ai/demo/run_gui.sh
# or headless level read-out:
./ai/tests/test_model.sh
```

A trained low tier should visibly gather the right stones and elevate. Levels
that need a real quorum (4→5 and up) only complete with several model drones on
one tile — which is the same hard rendezvous the scripted brain solves, and why
"first 4 levels by AI, rest by the scripted `-s` brain" is the pragmatic split.

## Caveats

- **Single-agent sim** ⇒ the model never sees real team-mates during training
  (the visible-player channel is always just itself). Good enough for the
  gather-heavy low tiers; higher tiers benefit from multi-agent or real-server
  fine-tuning.
- Keep the **architecture fixed** at `[64,64]`+Tanh unless you also change
  nothing else — the exporter and the existing models assume it.
- Re-run `export_mlp.py --selftest` after any change to `rl_agent.MlpModel`.
