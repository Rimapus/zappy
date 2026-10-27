# Usage

## Build

The project uses CMake with two presets: `debug` and `release`. The **release** preset enables LTO, `-O3`, `-march=native`, and `-Werror`.

```sh
# Configure (release)
cmake --preset release

# Build
cmake --build build/release

# The binary is placed at the source root: ./zappy_server
```

### Dependencies

- Clang++ with libc++ (`-stdlib=libc++`)
- C++23 standard

### Development Build

```sh
cmake --preset debug
cmake --build build/debug
```

## Run

```
./zappy_server -p port -x width -y height -n team1 team2 ... -c clientsNb -f freq
```

| Flag | Description |
|------|-------------|
| `-p` | Port number |
| `-x` | Map width |
| `-y` | Map height |
| `-n` | Team names (space-separated, at least one) |
| `-c` | Initial clients (eggs) per team |
| `-f` | Frequency (reciprocal of time unit for action execution) |
| `-h` / `--help` | Display usage |

### Example

```sh
./zappy_server -p 4242 -x 20 -y 20 -n team1 team2 -c 5 -f 100
```

## Tests

Unit tests use the [Criterion](https://github.com/Snaipe/Criterion) framework.

```sh
# Configure with tests enabled
cmake --preset debug -DBUILD_TESTS=ON

# Build the test executable
cmake --build build/debug --target zappy_tests

# Run all tests
./zappy_tests --verbose
```

Tests are **disabled by default** (`BUILD_TESTS=OFF`). The server build is unaffected when tests are off, and criterion is not required.

See [tests.md](tests.md) for a full description of the test suite.

## Lint

```sh
cmake --build build/release --target lint
```

Runs clang-tidy on all C++ source files (accelerated by clang-tidy-cache if available).
