# Build and run

## Server

Requirements: CMake 3.16+, Clang++ with libc++, C++23.

```sh
cmake -S server -B server/build -DCMAKE_BUILD_TYPE=Debug
cmake --build server/build -j
```

The server CMake hardcodes `-stdlib=libc++`; install the libc++ development
packages for your distribution.

## GUI

Requirements: CMake 3.16+, C++20, and the Linux development files for X11
(Xrandr, Xi and Xcursor), OpenGL, OpenAL, Vorbis, FLAC, FreeType and udev. The
first configure fetches SFML 3.0.0 from GitHub.

```sh
cmake -S gui -B gui/build -DCMAKE_BUILD_TYPE=Release
cmake --build gui/build -j
```

The GUI loads textures with paths relative to its working directory, so run it
from `gui/`. The asset directory is documented in
[`gui/assets/README.md`](gui/assets/README.md).

## AI

Requirements: CMake 3.16+, a C++20 compiler.

```sh
cmake -S ai -B ai/build -DCMAKE_BUILD_TYPE=Release
cmake --build ai/build -j
```

The AI uses the scripted policy by default. Set `ZAPPY_POLICY=model` to load the
exported MLP models; see [`ai/python/README.md`](ai/python/README.md) and
[`ai/python/training/README.md`](ai/python/training/README.md).

## Root make

The root `Makefile` builds each component through its CMake project and copies the
binaries to the repository root:

```sh
make zappy_server zappy_ai
make zappy_gui
```

The server and AI can run from the root. Run the GUI from `gui/` so it finds
`assets/`.

## Run

Start the server:

```sh
./server/zappy_server -p 4242 -x 12 -y 12 -n team1 team2 -c 3 -f 100
```

In another terminal, connect an AI:

```sh
./ai/zappy_ai -p 4242 -n team1 -h 127.0.0.1
```

With assets in place, connect the GUI:

```sh
cd gui
./zappy_gui -p 4242 -h 127.0.0.1
```

## Tests

The server has a Criterion test suite. Criterion must be installed and discoverable
by CMake:

```sh
cmake -S server -B server/build-tests -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
cmake --build server/build-tests -j
./server/zappy_tests
```

The test binary reports 216 tests; all passed when run against this source.
