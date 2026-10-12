# Nova

Nova is a readable scripting language implemented in C++17. Version 1 ships an interpreter, CLI, examples, documentation, and a CMake test suite.

## Build
Requirements: C++17 compiler and CMake 3.16+.
```sh
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build --output-on-failure
./build/nova examples/hello.nova
```
Windows multi-config builds commonly place the executable at `build/Release/nova.exe`.

## Hello Nova
```nova
say "Hello, world!"
let name = "Nova"
say "Welcome to " + name
let i = 1
while (i <= 3) {
  say i
  i = i + 1
}
if (i == 4) {
  say "It works!"
} else {
  say "Unexpected result"
}
```

## v1 features
- Numbers, strings, booleans, and nil
- Variables and assignment
- Arithmetic, comparisons, equality, logic, string concatenation
- say/print, if/else, while, block scopes
- # and // line comments
- File execution, interactive prompt, help/version flags
- Line-numbered syntax/runtime errors

Functions, collections, imports, package management, bytecode, and native compilation are future work, not v1 features. See [docs/LANGUAGE.md](docs/LANGUAGE.md).
