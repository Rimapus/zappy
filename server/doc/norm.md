# Coding Standards

## Include What You Use

Every source file must include all headers it directly uses. Do not rely on transitive includes. This policy is enforced by clang-tidy's `#include` checks and is verified during review.

## Build Configuration

| Setting | Value |
|---------|-------|
| Language | C++23 (`-std=c++23`) |
| Compiler | Clang (`clang++`) |
| Standard library | LLVM libc++ (`-stdlib=libc++`) |
| Extensions | Disabled (`-fno-extensions`) |
| Warnings | `-Wall -Wextra -Wpedantic` |
| Release | `-O3 -march=native -Werror` with LTO |
| Debug | `-O0` |

## Lint

```sh
cmake --build build/release --target lint
```

This runs `clang-tidy` (v20 preferred) on all `.cpp` and `.hpp` files in the source tree. If `clang-tidy-cache` is installed, it is used to accelerate repeated runs.

## Formatting

`.clang-format` and `.clang-tidy` configuration files are at the project root and are applied by the CI pipeline.

## General Guidelines

- Prefer `std::println` over `printf`/`std::cout`
- Use `std::format` for string formatting
- Use `std::to_underlying` for enum-to-integer conversion
- Use `#pragma once` for header guards
- Prefer `auto` return types for readability when applicable
- Use `std::unique_ptr` for ownership, `std::shared_ptr` for shared ownership
- Copy constructors are `delete`d for resource-owning classes
