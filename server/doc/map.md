# Map

## Map (`src/map/Map.hpp` and `Map.cpp`)

A rectangular grid of tiles with toroidal wrapping. Contains a list of eggs.

| Method | Description |
|--------|-------------|
| `width()` / `height()` | Grid dimensions |
| `at(x, y)` | Tile reference with bounds check |
| `countNaturalResources(type)` | Count naturally-spawned resources of a type across all tiles |
| `spawnNaturalResources(data)` | Replenish resources to meet density targets |
| `spawnEggAt(x, y, team, playerId)` | Create an egg |
| `spawnEggs(teams, count)` | Initial egg distribution |
| `spawnEggForTeam(team)` | Place one egg at the least-crowded position |
| `eggCountAt(x, y)` | Count eggs at tile |
| `eggCountForTeam(team)` | Count eggs for a team |
| `consumeEggForTeam(team)` | Remove and return `(eggId, x, y)` of a random team egg |

**Resource Spawning:** Uses pre-defined densities against total area. Spawns on tiles with the fewest resources (ties broken randomly).

## Tile (`src/map/Tile.hpp` and `Tile.cpp`)

A cell on the map holding resources.

| Method | Description |
|--------|-------------|
| `addResource(resource)` | Insert resource |
| `resourceCount()` | Total resources |
| `resourceCount(type, naturalOnly)` | Count by type, optionally filtering to natural |
| `getResources()` | Vector of all resource types |
| `hasResource(type, number)` | Check for at least N of a type |
| `takeResource(type)` | Remove and return one resource |
| `takeResources(type, nb)` | Remove and return up to N resources |

## Egg (`src/map/Egg.hpp`)

Represents a spawn point for a player.

| Field | Description |
|-------|-------------|
| `_id` | Auto-incrementing unique ID |
| `_x`, `_y` | Position |
| `_teamName` | Owning team |
| `_playerId` | Optional — set when created by a player (Fork) |

## Resources

### IResource (`src/map/resources/IResource.hpp`)

Abstract interface:

- `getType()` → `ResourceType`
- `isNatural()` / `setNatural()` — distinguishes spawned vs. player-placed

### ResourceType

Enum with 7 values + `COUNT` sentinel.

| Value | String |
|-------|--------|
| FOOD | `"food"` |
| LINEMATE | `"linemate"` |
| DERAUMERE | `"deraumere"` |
| SIBUR | `"sibur"` |
| MENDIANE | `"mendiane"` |
| PHIRAS | `"phiras"` |
| THYSTAME | `"thystame"` |

### AResource (`src/map/resources/AResource.hpp`)

Base implementation: stores type + natural flag.

### Concrete Types

All header-only, identical pattern:
- `Food`, `Linemate`, `Deraumere`, `Sibur`, `Mendiane`, `Phiras`, `Thystame` (in `src/map/resources/`)

### Resource Densities

| Resource | Density | Target per area |
|----------|---------|-----------------|
| Food | 0.50 | `max(1, area * 0.5)` |
| Linemate | 0.30 | `max(1, area * 0.3)` |
| Deraumere | 0.15 | `max(1, area * 0.15)` |
| Sibur | 0.10 | `max(1, area * 0.1)` |
| Mendiane | 0.10 | `max(1, area * 0.1)` |
| Phiras | 0.08 | `max(1, area * 0.08)` |
| Thystame | 0.05 | `max(1, area * 0.05)` |

### ResourcePtrCompare

Transparent comparator for `multiset<unique_ptr<IResource>>` that orders by `ResourceType::Value`, enabling heterogeneous lookup by `ResourceType::Value`.
