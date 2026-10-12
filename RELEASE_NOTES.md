# Nova v1 — Release Notes

Nova is an early C++17 interpreter for the Nova programming language.

## Included

- Run `.nova` source files from the command line
- Interactive REPL
- Numbers, strings, booleans, and `nil`
- Variables and reassignment
- Arithmetic, comparison, and logical operators
- `say` and `print` output statements
- `if` / `else`, `while`, block scopes, and comments
- CMake build configuration and a GitHub Actions CI workflow
- Example programs and a smoke test

## Build

```sh
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

## Run an example

```sh
./build/nova examples/hello.nova
```

On Windows, the executable may be under `build/Release/` depending on the generator.

## Current limitations

This is an early interpreter release. Functions, arrays/objects, imports, a package manager, bytecode, and native compilation are not implemented yet. Run the test suite on your platform before relying on this version.

## Release status

This file documents the v1 milestone in the repository. It is not itself a GitHub Release asset or a downloadable binary.