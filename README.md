# dsa-visualizer

This repository is a practice workspace for ThePrimeagen's DSA course in C++20
with a working terminal visualizer.

## Workflow

When solving exercises, work in `src/dsa/`.

- Edit TODO stubs in `src/dsa/`
- Keep `src/viz/` as-is for visualization/runtime behavior

Run commands from repo root (`/path-to/dsa-visualizer`).

### 1) Build once

```bash
cmake -S . -B build
cmake --build build --target dsa_learner_tests
```

### 2) Choose tests

```bash
./build/tests/dsa_learner_tests --gtest_list_tests
```

### 3) Run a single test

```bash
./build/tests/dsa_learner_tests --gtest_filter=LinearSearch.FindsExistingElement
```

### 4) Run one full suite

```bash
./build/tests/dsa_learner_tests --gtest_filter='LinearSearch.*'
```

### 5) Run all learner tests

```bash
./build/tests/dsa_learner_tests
```

## Run the Visualizer

```bash
cmake --build build --target dsa-viz
./build/apps/dsa-viz
```

Core controls in the visualizer:

- `Space`: run/pause
- `N` / `B`: step forward/backward
- `R`: reset
- `+` / `-`: speed visualization up/down
- `Q` or `Esc`: quit

## Default Project Tests

```bash
ctest --test-dir build --output-on-failure
```

Note: `ctest` runs default visualizer tests (`dsa_tests`). Learner tests are run
directly with `./build/tests/dsa_learner_tests`.

## Troubleshooting

- If `ctest` says `No tests were found!!!` for learner suites, use the learner
  binary directly:
  - `./build/tests/dsa_learner_tests --gtest_filter='LinearSearch.*'`
- In `zsh`, quote wildcard filters like `'LinearSearch.*'`.

## Technical Notes

- C++20
- CMake 3.20+
