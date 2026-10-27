# Elevation

## Requirements per Level

An incantation requires a specific number of players and resources on the same tile.

| Target Level | Players | Linemate | Deraumere | Sibur | Mendiane | Phiras | Thystame |
|:------------:|:-------:|:--------:|:---------:|:----:|:--------:|:-----:|:--------:|
| 1→2 | 1 | 1 | — | — | — | — | — |
| 2→3 | 2 | 1 | 1 | 1 | — | — | — |
| 3→4 | 2 | 2 | — | 1 | — | — | — |
| 4→5 | 4 | 1 | 1 | 2 | — | 1 | — |
| 5→6 | 4 | 1 | 2 | 1 | 3 | — | — |
| 6→7 | 6 | 1 | 2 | 3 | — | 1 | — |
| 7→8 | 6 | 2 | 2 | 2 | 2 | 2 | 1 |

## Incantation Flow

1. **Initiation** (instant, 0 time units):
   - Check tile has required resources and enough eligible players (same tile, same level, not frozen)
   - Select a balanced group (spread across teams, initiator's team prioritized)
   - Freeze all participants → they receive `Elevation underway`
   - Broadcast `pic x y l #id1 #id2 ...` to GUI
   - Schedule completion event at +300 time units

2. **Completion** (+300 time units):
   - Verify tile still has requirements and all participants are alive
   - If failed → `pie x y 0\n`, send `ko`, unfreeze
   - If success → consume resources from tile, level each participant to current + 1, broadcast `pie x y 1\n`
   - Unfreeze all participants, resume their event queues

## Win Condition

After each successful level-up from 7 to 8, the server checks:

> If a team has **6 or more players at level 8**, the game ends and that team wins.

The team with the most level 8 player wins.

Ties are possible: all teams reaching the same maximum count of level-8 players are announced as winners via `seg team\n`.
