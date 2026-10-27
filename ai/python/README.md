# Zappy AI - RL agent (one model per level)

`rl_agent.py` is the Python side of the AI's decision brain. The C++ client
(`ModelPolicy`, over `RlBridge`) launches it as a child process and exchanges
one line each way:

```
stdin  <- one JSON observation per line   (produced by Serializer.cpp)
stdout -> one protocol command per line    ("Forward", "Take linemate", ...)
```

The observation carries the player's `level`; the agent runs the model trained
for that level - this is the "one model per level" part. No third-party Python
dependency is required (pure-Python forward pass).

## Models

Models live in the directory passed as `argv[1]` (default `ai/models`). The
trained set has one model per tier (`tier2`..`tier7`); the player level is mapped
to a tier with `tier = min(max(level, 2), 7)`, so `tier2` also covers level 1 and
`tier7` also covers level 8. For a given level, the first of these that exists is
used (real exported names first):

```
zappy_ppo_tier<T>_best.mlp   zappy_ppo_tier<T>.mlp   tier<T>.mlp   level_<L>.mlp   policy.mlp
```

Format: `ZAPPY_MLP` v1 (see `MlpModel.load`). If no model is found, the agent
emits a safe default (`Forward`) so the client keeps running.

Three things must stay in sync with the training pipeline (all clearly marked in
`rl_agent.py`): `ACTIONS` (action space), `encode_observation` (input encoding),
and `model_path` (file naming).

## Switching brains (model vs scripted)

The C++ side is policy-agnostic - switching the brain never touches the game loop
(`ZappyAI::run`). Pick the policy with the `ZAPPY_POLICY` env var:

```
ZAPPY_POLICY=model      # default - RL models via this agent
ZAPPY_POLICY=scripted   # reserved; add a ScriptedPolicy : IPolicy (see Policy.cpp)
```

Override the agent command with `ZAPPY_RL_CMD`, e.g.:

```
ZAPPY_RL_CMD="python3 ai/python/rl_agent.py /path/to/models"
```

Adding a scripted brain later is just: implement `ai::ScriptedPolicy : IPolicy`,
return it from `makePolicy(PolicyKind::Scripted, ...)`. The interface is in
`ai/src/policy/IPolicy.hpp`.
