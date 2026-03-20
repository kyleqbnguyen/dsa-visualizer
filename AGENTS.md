# AGENTS.md
Guidance for coding agents working in `dsa-visualizer`.

## Project Snapshot
- Language: C++20
- Build system: CMake (min 3.20)
- Test framework: GoogleTest with `gtest_discover_tests`
- UI stack: FTXUI via `FetchContent`
- Main executable: `dsa-viz`
- Test executables:
  - `dsa_tests` (default visualizer tests)
  - `dsa_learner_tests` (optional learner tests)
- Key directories:
  - `src/dsa/` algorithms (`INTERFACE` target)
  - `src/viz/` visualization/controller (`INTERFACE` target)
  - `apps/` entrypoint (`main.cpp`)
  - `tests/` unit tests

## Source-Of-Truth Files
- `CMakeLists.txt`
- `.clang-format`
- `apps/CMakeLists.txt`
- `src/dsa/CMakeLists.txt`
- `src/viz/CMakeLists.txt`
- `tests/CMakeLists.txt`

## Build / Run / Test / Lint
Run commands from repo root: `/Users/blue/personal/dsa-visualizer`.

### Configure
```bash
cmake -S . -B build
```

### Build all
```bash
cmake --build build
```

### Build one target
```bash
cmake --build build --target dsa-viz
cmake --build build --target dsa_tests
```

### Configure with learner tests enabled (optional)
```bash
cmake -S . -B build -DDSA_ENABLE_LEARNER_TESTS=ON
```

### Build learner test target (optional)
```bash
cmake --build build --target dsa_learner_tests
```

### Run app
```bash
./build/apps/dsa-viz
```

### Run all tests
```bash
ctest --test-dir build --output-on-failure
```

### Run a single test (preferred)
```bash
ctest --test-dir build -R '^VizRecordings\.BubbleSortRecordingEndsSorted$' --output-on-failure
```

### Run one suite
```bash
ctest --test-dir build -R '^VizRecordings\.' --output-on-failure
```

### Run learner tests explicitly (when enabled)
```bash
ctest --test-dir build -R '^LinearSearch\.|^BinarySearch\.|^BubbleSort\.' --output-on-failure
```

### Run single test via GoogleTest filter (alternative)
```bash
./build/tests/dsa_tests --gtest_filter=VizRecordings.BubbleSortRecordingEndsSorted
```

### Run single learner test via GoogleTest filter (optional)
```bash
./build/tests/dsa_learner_tests --gtest_filter=BinarySearch.FindsFirstElement
```

### List discovered tests
```bash
ctest --test-dir build -N
```

### Format (apply)
```bash
git ls-files '*.h' '*.cpp' | xargs clang-format -i
```

### Format (check only)
```bash
git ls-files '*.h' '*.cpp' | xargs clang-format --dry-run --Werror
```

### Lint notes
- No dedicated `clang-tidy`/`cpplint` target is configured
- Use `clang-format --dry-run --Werror` as the lint gate

## Coding Style Guidelines

### Formatting
- `.clang-format` values:
  - `BasedOnStyle: LLVM`
  - `ColumnLimit: 80`
  - `IndentWidth: 2`
  - `UseTab: Never`
- Keep short guard clauses compact when readable
- Prefer trailing return syntax for longer signatures

### Headers, Files, Includes
- Use `#pragma once` in headers
- Keep inline/template implementations in headers when matching existing code
- Include order with blank-line groups:
  1) standard library
  2) third-party (`ftxui/...`, `gtest/...`)
  3) project headers (`"*.h"`)
- Keep includes sorted within each group where practical

### Namespaces and Naming
- Use `namespace dsa` for algorithm code
- Use `namespace viz` for visualization/runtime code
- Use anonymous namespace in `.cpp` for file-local helpers
- Types: `PascalCase` (`StepSnapshot`, `VizController`)
- Functions/variables: `snake_case` (`run_visualizer`, `sorted_boundary`)
- Constants: `kPascalCase` (`kBinarySearch`, `kTickMs`)
- Enum values are uppercase in this codebase (`CONTINUOUS`, `STEP`, `PAUSED`)

### Types and APIs
- Use `std::size_t` for container indexing in algorithms
- Use `int` for sentinel UI state where `-1` means unset
- Use `std::optional<T>` for maybe-found results
- Use `std::span` for non-owning sequence views
- Use constrained templates when practical (`std::equality_comparable`, `std::totally_ordered`)
- Use explicit `static_cast` at signed/unsigned boundaries

### Error Handling and Validation
- Prefer non-throwing flows (`bool`, `optional`, validation strings)
- Validate inputs early (empty data, parse failures, invalid config)
- In UI/config code, surface errors with user-facing `validation_msg`
- Keep algorithm stepping deterministic and reproducible

### State, Concurrency, Runtime
- `VizController` uses atomics with `memory_order_relaxed`; keep consistent
- Keep tick/update loops lightweight and non-blocking
- Post UI refresh events (`screen.Post(Event::Custom)`) after state changes

### Tests
- Add/update tests in `tests/test_*.cpp` for behavior changes
- Prefer focused deterministic cases
- Always cover edge cases (empty, single-element, not-found, sorted/reverse-sorted)
- For callback-enabled algorithms, include at least one callback test

## Dependencies and CMake Notes
- Dependencies fetched by `FetchContent`:
  - FTXUI `v6.1.9`
  - GoogleTest `v1.15.2`
- `dsa` and `viz` are `INTERFACE` libraries exporting include dirs
- Default tests are auto-registered by `gtest_discover_tests(dsa_tests)`
- Learner tests are conditionally registered by `gtest_discover_tests(dsa_learner_tests)`

## Agent Workflow
- Before editing, inspect related headers/tests to match local conventions
- After edits, run at minimum:
  1) `cmake --build build`
  2) `ctest --test-dir build --output-on-failure`
  3) `git ls-files '*.h' '*.cpp' | xargs clang-format --dry-run --Werror`
- For new algorithms, typically:
  - add implementation in `src/dsa/`
  - add recorder/view integration in `src/viz/`
  - wire menu + launcher in `apps/main.cpp`
  - add tests in `tests/`

## Cursor / Copilot Rule Files
- No Cursor rule files found:
  - `.cursorrules` absent
  - `.cursor/rules/` absent
- No Copilot instruction file found:
  - `.github/copilot-instructions.md` absent

If these files are added later, update this document accordingly.
