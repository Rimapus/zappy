# Architecture

## Overview

The server is a single-threaded, event-driven C++23 application. It multiplexes I/O over all connected clients using `poll()` and schedules game actions via a priority queue of timed events.

```
main()
  ├─ parseArguments() → Config
  └─ server::run(config)
       ├─ initData() → ServerData
       │    ├─ Create server Socket
       │    ├─ Allocate Map
       │    ├─ Spawn natural resources
       │    └─ Spawn eggs per team
       ├─ Main loop (until game finished)
       │    ├─ processPeriodicEvents()
       │    ├─ mainPoll() — poll() all sockets with timeout
       │    ├─ handlePollRevent()
       │    │   ├─ Handle GUI clients
       │    │   ├─ Handle active (AI) clients
       │    │   ├─ Handle pending clients
       │    │   └─ Handle server socket (accept new)
       │    └─ handleGameEvents()
       │         └─ Pop & execute expired GameEvents
       └─ Broadcast "smg Server is closing"
```

## Client Lifecycle

A connection progresses through three states:

1. **Pending** — unauthenticated; must send a team name or `GRAPHIC`
2. **Active** — authenticated as an AI player; can send game commands
3. **GUI** — authenticated as a spectator; receives state events and can query

```
Client connects
  │
  ▼
Pending ──(team name)──► Active ──(disconnect)──► Removed
  │
  └──(GRAPHIC)──► GUI
```

## Event Scheduling

Actions are implemented as `GameEvent` objects with a callback and a time cost. The cost is divided by the server frequency to compute a real-time delay. Events are stored in a min-heap priority queue ordered by execution time.

A player can have up to **10 pending events** (1 being executed and at most 9 waiting in a list). When a player is frozen (during elevation), their next event is paused and later rescheduled.

## Periodic Events

Two recurring timers drive game mechanics:

| Event | Interval (time units) | Action |
|-------|-----------------------|--------|
| Resource spawn | 20 | Replenish natural resources toward density targets |
| Food consumption | 126 | Remove one Food from every player's inventory; kill if none |
