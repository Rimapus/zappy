# Tests

## Unit tests

| File | Tests |
|------|-------|
| `tests/test_Tile.cpp` | Resource add, count, has, take operations (17) |
| `tests/test_Map.cpp` | Map dimensions, bounds checking, egg management (18) |
| `tests/test_Player.cpp` | Movement wrapping (4 directions), turns, default state (20) |
| `tests/test_ResourceType.cpp` | `toString` / `fromString` for all resource types (20) |
| `tests/test_StringUtils.cpp` | `toLowerCase` edge cases (5) |
| `tests/test_Elevation.cpp` | Elevation requirements data integrity (8) |
| `tests/test_Enum.cpp` | `EnumRange` iteration behavior (4) |
| `tests/commands/test_Forward.cpp` | Movement in all 4 directions, wrapping, args validation (9) |
| `tests/commands/test_Right.cpp` | Direction cycling, args validation (3) |
| `tests/commands/test_Left.cpp` | Direction cycling, args validation (3) |
| `tests/commands/test_Look.cpp` | Self-view, nearby player detection, args validation (3) |
| `tests/commands/test_Inventory.cpp` | Inventory output format, args validation (2) |
| `tests/commands/test_Take.cpp` | Resource pickup from tile, missing resource, multiple resources (5) |
| `tests/commands/test_Set.cpp` | Resource placement on tile, inventory validation, unknown resource (4) |
| `tests/commands/test_Eject.cpp` | Player ejection, empty tile, args validation (3) |
| `tests/commands/test_Broadcast.cpp` | Message passing between players, args validation (3) |
| `tests/commands/test_ConnectNbr.cpp` | Egg count, unknown team, args validation (4) |
| `tests/commands/test_Fork.cpp` | Egg spawning at position, args validation (2) |
| `tests/commands/test_Elevation.cpp` | Full elevation flow: resource check, player count, success, mid-elevation theft (5) |
| `tests/gui_commands/test_Bct.cpp` | Tile content string, valid/invalid/out-of-bounds coordinates (6) |
| `tests/gui_commands/test_Mct.cpp` | All-tile content broadcast, args validation (2) |
| `tests/gui_commands/test_Msz.cpp` | Map size reply, args validation (2) |
| `tests/gui_commands/test_Tna.cpp` | Team name list, args validation (2) |
| `tests/gui_commands/test_Sgt.cpp` | Frequency reply, args validation (2) |
| `tests/gui_commands/test_Sst.cpp` | Frequency update, zero rejection, args validation (3) |
| `tests/gui_commands/test_Ppo.cpp` | Player position query, not-found, args validation (3) |
| `tests/gui_commands/test_Plv.cpp` | Player level query, not-found, args validation (3) |
| `tests/gui_commands/test_Pin.cpp` | Player inventory query, not-found, args validation (3) |

**All 164 tests pass.**

## Coverage

Code coverage is available via [gcovr](https://gcovr.com/).

```sh
# Configure with coverage enabled (implies BUILD_TESTS=ON)
cmake -S . -B build/debug -DBUILD_TESTS=ON -DBUILD_COVERAGE=ON -DCMAKE_CXX_COMPILER=clang++

# Build, run tests, and generate HTML report
cmake --build build/debug --target coverage
```

This produces `coverage.html` (with per-file breakdowns) at the project root.
Coverage is **disabled by default** (`BUILD_COVERAGE=OFF`).

```sh
# Clean coverage artifacts and instrumentation data
cmake --build build/debug --target coverage-clean
```

### Prerequisites

- **Linux:** `apt install gcovr llvm` (uses `llvm-cov` as the gcov backend)
- **macOS:** `brew install gcovr llvm`

## Test infrastructure

Tests use real socket pairs (`socketpair(AF_UNIX, SOCK_STREAM, 0, ...)`) to create connected file
descriptors for each `Client`, then pass a `ServerData` with a dummy server socket.  Helper
functions in `tests/test_helpers.hpp` provide:

- `createDummySocket()` — server socket placeholder
- `createTestServerData(freq, width, height, teams)` — ready-to-use `ServerData`
- `createTestClient()` — `socketpair`, wraps one end in `Client`, returns `{client, testFd}`
  (discards the welcome message)
- `addToActiveClients(data, client)` — registers client in the server's active list
- `executeGameEvent(data)` — pops the next scheduled `GameEvent` and runs it
- `readAll(fd)` — non-blocking drain of the test fd (safe after a blocking `writeToClient`)
- `closeTestFd(fd)` — closes the test end of the socketpair

## Tested classes / handlers

### Core classes
- **Player** — direction cycling, forward wrapping (toroidal map), freeze state, ID auto-increment, default inventory (10 food), event scheduling
- **Tile** — resource insertion, type-specific counting, natural-only filtering, ownership transfer on take
- **Map** — bounds checking (`at` throws `std::out_of_range`), egg lifecycle (spawn, count, consume), toroidal wrapping validation
- **ResourceType** — bidirectional string mapping for all 7 types, invalid input rejection
- **StringUtils** — locale-agnostic lowercase conversion
- **Elevation requirements** — exact player counts and resource amounts per level (1–7)
- **Enum utilities** — `EnumRange` covers all values from `FOOD` to `COUNT` (exclusive)

### AI command handlers
- **Forward** — `handleForward`: moves player one tile in facing direction with toroidal wrapping
- **Right/Left** — `handleRight` / `handleLeft`: rotates player direction clockwise / counter-clockwise
- **Look** — `handleLook`: builds `[tile,...]` string of visible tiles (includes empty tiles and other players)
- **Inventory** — `handleInventory`: returns `[food N, linemate N, ...]` with all 7 resource counts
- **Take** — `handleTake`: transfers resource from tile to player inventory via scheduled event
- **Set** — `handleSet`: transfers resource from player inventory to tile via scheduled event
- **Eject** — `handleEject`: pushes other players off the current tile in a random direction
- **Broadcast** — `handleBroadcast`: sends a message to all players at a computed sound direction
- **ConnectNbr** — `handleConnectNbr`: returns number of unused slots (egg count + team slots) synchronously
- **Fork** — `handleFork`: creates an egg at the player's current position via scheduled event
- **Elevation** — `handleElevation`: full incantation protocol — validates args synchronously, then
  schedules two events (initiation + completion). Tests cover: bad args, insufficient resources,
  insufficient players, successful level-up, and resource theft mid-ritual

### GUI command handlers
- **Bct** — `handleBct` / `getTileContentString`: returns tile resource counts (7 resources)
- **Mct** — `handleMct`: broadcasts `bct` for all map tiles
- **Msz** — `handleMsz`: returns map dimensions
- **Tna** — `handleTna`: returns registered team names
- **Sgt** — `handleSgt`: returns current frequency
- **Sst** — `handleSst`: updates frequency (rejects zero)
- **Ppo** — `handlePpo`: returns player position and orientation
- **Plv** — `handlePlv`: returns player level
- **Pin** — `handlePin`: returns player inventory (all 7 resources + food)
