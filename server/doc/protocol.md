# Protocol

## AI Commands

Sent by AI clients after authentication. Commands are dispatched via the `COMMANDS` map.

| Command | Time | Description |
|---------|------|-------------|
| `Forward` | 7 | Move one tile forward in facing direction |
| `Right` | 7 | Turn 90° clockwise |
| `Left` | 7 | Turn 90° counter-clockwise |
| `Look` | 7 | View tiles in vision cone (size = level) |
| `Inventory` | 1 | List resources in inventory |
| `Take object` | 7 | Pick up resource from current tile |
| `Set object` | 7 | Drop resource on current tile |
| `Broadcast text` | 7 | Send message to all players with direction-of-arrival |
| `Connect_nbr` | 0 | Return number of remaining eggs for player's team |
| `Fork` | 42 | Lay an egg at current position |
| `Eject` | 7 | Push all players and destroy eggs on current tile |
| `Incantation` | 0 + 300 | Start elevation ritual (instant check, 300tu completion) |

### Connection Sequence

```
Server:  WELCOME\n
Client:  <team_name>\n    (or GRAPHIC)
Server:  <remaining_eggs>\n<width> <height>\n

upon authentication:
  Server broadcasts: pnw #id x y o l team\n
```

## GUI Commands

Sent by GUI clients. Dispatched via `GUI_COMMANDS` map. Unknown commands receive `suc\n`. Bad arguments receive `sbp\n`.

| Command | Response | Description |
|---------|----------|-------------|
| `msz` | `msz X Y\n` | Map size |
| `bct X Y` | `bct X Y q0 q1 q2 q3 q4 q5 q6\n` | Tile content (7 resource counts) |
| `mct` | `bct ...` × all tiles | All tile contents |
| `tna` | `tna NAME\n` per team | Team names |
| `ppo #N` | `ppo #N X Y O\n` | Player position + orientation (1-4) |
| `plv #N` | `plv #N L\n` | Player level |
| `pin #N` | `pin #N X Y q0...q6\n` | Player inventory |
| `sgt` | `sgt F\n` | Time frequency |
| `sst F` | `sst F\n` | Set time frequency (updates server) |

### Resource order (q0..q6)

`food linemate deraumere sibur mendiane phiras thystame`

## Server-to-GUI Events

| Event | Format | Description |
|-------|--------|-------------|
| pnw | `pnw #id x y o l team\n` | New player connected |
| pex | `pex #id\n` | Player ejected |
| pbc | `pbc #id msg\n` | Player broadcast |
| pic | `pic x y l #id1 #id2 ...\n` | Incantation started |
| pie | `pie x y r\n` | Incantation ended (r=0 fail, 1 success) |
| pfk | `pfk #id\n` | Player forking |
| pdr | `pdr #id r\n` | Player dropped resource |
| pgt | `pgt #id r\n` | Player took resource |
| pdi | `pdi #id\n` | Player died/disconnected |
| enw | `enw #eid #pid x y\n` | Egg laid |
| ebo | `ebo #eid\n` | Egg hatched (player born) |
| edi | `edi #eid\n` | Egg destroyed (eject) |
| seg | `seg team\n` | Game ended, team wins |
| smg | `smg msg\n` | Server message |
| suc | `suc\n` | Unknown GUI command |
| sbp | `sbp\n` | Bad GUI command parameters |
| bct | `bct x y q0...q6\n` | Tile update |
| ppo | `ppo #id x y o\n` | Player position update |
| plv | `plv #id l\n` | Player level update |
| pin | `pin #id x y q0...q6\n` | Player inventory update |

### Resource ID mapping (r in pdr/pgt)

| ID | Resource |
|----|----------|
| 0 | Food |
| 1 | Linemate |
| 2 | Deraumere |
| 3 | Sibur |
| 4 | Mendiane |
| 5 | Phiras |
| 6 | Thystame |
