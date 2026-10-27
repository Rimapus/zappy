# Communication Layer

## Socket (`src/communication/Socket.hpp` and `Socket.cpp`)

RAII wrapper around a file descriptor. Move-only (copy disabled).

| Method | Description |
|--------|-------------|
| `Socket(domain, type, protocol)` | Create new socket via `::socket()` |
| `Socket(fd)` | Wrap an existing fd (validates it's a socket via `fstat`) |
| `~Socket()` | `::close(fd)` if valid |
| `bind(addr)` | `::bind()` |
| `setReuseAddress()` | `SO_REUSEADDR` |
| `listen(backlog=4096)` | `::listen()` |
| `accept()` | `::accept()`, returns `pair<Socket, sockaddr>` |
| `read(size)` | Poll for `POLLIN`, then `System::read()` |
| `write(str)` | Poll for `POLLOUT`, then `System::write()` |
| `poll(events, timeout)` | Single-fd poll via `System::poll()` |
| `pollMultiple(sockets, events, timeout)` | Static — poll a vector of sockets; returns `vector<optional<short>>` |

**Exceptions:**

| Exception | When |
|-----------|------|
| `SocketException` | Socket syscall failures (socket, bind, listen, accept, setsockopt) |
| `SocketClosed` | Remote peer closed connection (empty read or POLLERR/POLLHUP) |

## System (`src/System.cpp` and `common/System.hpp`)

Low-level I/O wrappers with proper error handling.

| Static Method | Description |
|---------------|-------------|
| `poll(fd, events, timeout)` | Single fd poll |
| `poll(list, timeout)` | Multiple fd poll |
| `write(fd, str)` | Full write with EINTR retry |
| `read(fd, size)` | Read up to `size` bytes with EINTR retry; returns empty string on EOF |

`SystemException` carries both an error message and `errno`.
