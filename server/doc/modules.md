# Modules

## Config (`src/Main.hpp`)

Command-line argument container.

| Field | Type | Description |
|-------|------|-------------|
| `_port` | `uint16_t` | Server port |
| `_width` | `size_t` | Map width |
| `_height` | `size_t` | Map height |
| `_teamNames` | `vector<string>` | Team names |
| `_clientsNumber` | `size_t` | Initial clients per team |
| `_frequency` | `size_t` | Execution frequency |

## ServerData (`src/Data.hpp`)

Central state hub owned by the main loop. Provides access to all subsystems.

| Accessor | Returns |
|----------|---------|
| `getServerSocket()` | Shared pointer to server listen socket |
| `getTeams()` | Team name list |
| `getClientsData()` | `ClientsData` (pending + active client vectors) |
| `getGameEvents()` | `priority_queue<GameEvent>` |
| `getMap()` | `Map` reference |
| `getFrequency()` | Current frequency |
| `getPeriodicEvents()` | `PeriodicEventsData` |
| `getGui()` | `Gui` manager |
| `getFinished()` / `setFinished()` | Game-over flag |

`setFrequency()` updates the tick rate and reschedules all pending events to the new timing.

## Client (`src/Client.hpp` and `Client.cpp`)

Wraps a connected socket with buffered I/O.

| Method | Description |
|--------|-------------|
| `writeToClient(str)` | Queue string for sending |
| `readFromClient()` | Read up to 4 KiB into buffer; close if no data |
| `getLineFromBuffer()` | Extract one newline-delimited line |
| `shouldClose()` / `kill()` | Connection management |
| `hasPlayer()` / `initPlayer()` | Player lifecycle |
| `isGui()` / `setGui()` | GUI client flag |
| `close(data)` | Drop player items on tile, broadcast `pdi`, write `dead` |

**Public functions:**

- `handlePendingClients(data, client)` — authenticates against team names or `GRAPHIC`
- `handleActiveClients(data, client)` — dispatches AI commands via `COMMANDS` map

Upon successful authentication, the server sends `{remainingEggs}\n{width} {height}\n`.

## Player (`src/Player.hpp` and `Player.cpp`)

Represents an in-game character.

| Property | Type |
|----------|------|
| Direction | enum { North, East, South, West } |
| Position | `_x`, `_y` (toroidal map) |
| Level | 1–8 |
| Team | string |
| Inventory | `multiset<unique_ptr<IResource>>` |
| Event queue | max 9 pending events + 1 paused slot |

**Movement:** `moveForward(map)` wraps toroidally. `turnRight()` / `turnLeft()` rotate 90°.

**Event management:** `addNewEvent()` enqueues a game event. `scheduleNextEvent()` pops and schedules one from the queue. `pauseEvent()` / `resumeEvents()` handle freezing during elevation. A frozen player cannot schedule new events.

## GameEvent (`src/GameEvent.hpp`)

Represents a delayed action.

| Member | Description |
|--------|-------------|
| `_callback` | `function<void()>` to execute |
| `_timeLimit` | Cost in time units |
| `_executionTime` | Computed real-time deadline |
| `_scheduledAt` | Original scheduling time (used for rescheduling) |
| `_owner` | `weak_ptr<Client>` (owner for freeze-check) |

`scheduledTask(data)` computes deadline = `from + timeLimit / frequency`. Rescheduling reuses the original scheduling time. The `CompareEventTime` comparator orders events by earliest execution.

## Gui (`src/Gui.hpp` and `Gui.cpp`)

Manages connected GUI clients (spectators).

| Method | Description |
|--------|-------------|
| `addClient(client)` | Add to client list |
| `broadcast(string)` | Send raw string to all GUI clients |
| `broadcast(data, handler, args)` | Call handler for each GUI client |
| `getClients()` | Access the client vector |

`handleGuiClients(data, client)` dispatches GUI commands via `GUI_COMMANDS` map.

## PeriodicEvents (`src/PeriodicEvents.hpp` and `PeriodicEvents.cpp`)

Tracks which periodic timers need re-scheduling.

| Flag | Initial | Triggers |
|------|---------|----------|
| `_refilled` | `true` | Schedule resource spawn (20 tu) |
| `_foodConsumed` | `true` | Schedule food consumption (126 tu) |

`processPeriodicEvents()` checks and re-schedules expired timers in the main loop.

## Common Utilities (`common/`)

- **`StringUtils.hpp`** — `toLowerCase(string)` helper
- **`Enum.hpp`** — `ContinuousEnum` concept + `EnumIterator<E>` + `EnumRange<E>` for iterating enums with `COUNT` sentinel
- **`System.hpp`** — Forward declaration of static I/O methods (see [communication.md](communication.md))
