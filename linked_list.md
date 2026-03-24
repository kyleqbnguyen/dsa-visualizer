# Linked List Visualization — Technical Implementation Guide

## Confirmed Scope

- **List types**: Singly linked list and doubly linked list
- **Operations**: Prepend, Append, Insert At index, Remove At index, Get/search
- **Step granularity**: Every node visited during traversal is a frame, plus each pointer-rewire step
- **Menu**: One entry per list type; operation selected via the `C` config overlay
- **Traversal direction**: Always from head
- **Initial data**: Hardcoded test-case scenarios (no random generation for linked lists)

---

## Files to Create

```
src/dsa/singly_linked_list.h
src/dsa/doubly_linked_list.h
src/viz/list_snapshot.h
src/viz/list_recorder.h
src/viz/list_code_panel.h
src/viz/list_config.h
src/viz/list_viz.h
tests/test_singly_linked_list.cpp
tests/test_doubly_linked_list.cpp
tests/test_list_viz_recordings.cpp
```

## Files to Modify

```
src/viz/test_cases.h       — add linked list test case sets
apps/main.cpp              — wire menu entries
tests/CMakeLists.txt       — add new test files
```

---

## 1. DSA Stubs — `src/dsa/`

### `singly_linked_list.h`

Header-only, in `dsa::` namespace, matches the raw-pointer style from the course.

```cpp
namespace dsa {

template <typename T>
struct SinglyNode {
  T val;
  SinglyNode<T>* next = nullptr;
  explicit SinglyNode(T v) : val(v) {}
};

template <typename T>
class SinglyLinkedList {
public:
  SinglyLinkedList() = default;
  ~SinglyLinkedList();          // must free all nodes

  void   prepend(T val);
  void   append(T val);
  void   insert_at(size_t index, T val);
  void   remove_at(size_t index);
  T      get(size_t index) const;
  size_t length() const { return length_; }

private:
  SinglyNode<T>* head_   = nullptr;
  SinglyNode<T>* tail_   = nullptr;
  size_t         length_ = 0;
};
```

The list maintains both `head_` and `tail_` pointers so append is O(1).

### `doubly_linked_list.h`

Same interface, same structure, additionally adds:

```cpp
template <typename T>
struct DoublyNode {
  T val;
  DoublyNode<T>* next = nullptr;
  DoublyNode<T>* prev = nullptr;
  explicit DoublyNode(T v) : val(v) {}
};
```

`DoublyLinkedList<T>` exposes the same five methods. The traversal direction remains
head → tail for this stage (the "choose closer end" optimization is out of scope).

---

## 2. New Snapshot Type — `src/viz/list_snapshot.h`

Linked list state cannot be expressed with the existing `StepSnapshot` (which is
bar-chart oriented). A new, parallel type is introduced.

```cpp
namespace viz {

enum class ListOp { kPrepend, kAppend, kInsertAt, kRemoveAt, kGet };

enum class ListNodeState {
  kNormal,   // default
  kActive,   // traversal cursor currently here
  kNew,      // newly allocated node (not yet linked)
  kTarget,   // the node being inserted before / removed / found
  kFound,    // final result of a Get
  kRemoved,  // node unlinked but not yet freed (shown briefly)
};

struct ListNodeSnap {
  int           value = 0;
  ListNodeState state = ListNodeState::kNormal;
  bool          has_next = false;
  bool          has_prev = false;   // only meaningful for doubly linked
};

struct ListStepSnapshot {
  // nodes[0] is always head (or the pending new node for prepend)
  std::vector<ListNodeSnap> nodes;

  std::string status_text;
  std::string trace_entry;
  int         current_line = -1;
  std::vector<std::pair<std::string, std::string>> variables;
};

struct ListAlgorithmRecording {
  std::string               title;
  std::string               algorithm_name;   // e.g. "singly_linked_list"
  ListOp                    op;
  std::vector<ListStepSnapshot> steps;
};

} // namespace viz
```

### Encoding the list in `nodes`

The recorder materialises the linked list into a flat `vector<ListNodeSnap>` for each
snapshot. The position in the vector is the node's logical index (0 = head). Pointer
relationships are encoded as `has_next` / `has_prev` booleans on each element. This keeps
snapshots self-contained and decouples the renderer from pointer arithmetic.

During a rewire step, `has_next` of the relevant node is toggled to reflect the state
*after* the pointer change, so the renderer shows the updated topology.

---

## 3. Recording Functions — `src/viz/list_recorder.h`

One function per operation per list type (10 total). All follow the same pattern as
`record_linear_search`: simulate the operation imperatively, push one `ListStepSnapshot`
per significant event.

### Common input signature

```cpp
inline auto record_singly_prepend(const std::vector<int>& initial, int val)
    -> ListAlgorithmRecording;

inline auto record_singly_append(const std::vector<int>& initial, int val)
    -> ListAlgorithmRecording;

inline auto record_singly_insert_at(const std::vector<int>& initial,
                                    size_t index, int val)
    -> ListAlgorithmRecording;

inline auto record_singly_remove_at(const std::vector<int>& initial,
                                    size_t index)
    -> ListAlgorithmRecording;

inline auto record_singly_get(const std::vector<int>& initial, size_t index)
    -> ListAlgorithmRecording;

// Doubly linked equivalents follow the same signatures.
```

The `initial` vector is treated as the starting node values from head to tail.

### Step breakdown per operation

**Prepend(val)**
1. Initial snapshot — list as-is, all nodes `kNormal`
2. New node created (`kNew`) prepended to the left — `has_next = false` yet
   - `trace_entry`: `"Node* node = new Node(val)"`
3. `node->next = head` — new node gains `has_next = true`, pointing to old head
   - `trace_entry`: `"node->next = head"`
4. `head = node` — new node state → `kFound`, becomes permanent head
   - `trace_entry`: `"head = node; ++length"`
5. Final snapshot — stable list, new head highlighted `kFound`

**Append(val)**
1. Initial snapshot
2. `curr = head`, curr node → `kActive`
3. For each node until `curr->next == nullptr`: advance curr, previous → `kNormal`, next → `kActive`
   - `trace_entry`: `"curr = curr->next"`
4. New node created (`kNew`) appended after tail — `has_next = false`
5. `tail->next = node` — tail gains `has_next = true`
   - `trace_entry`: `"curr->next = node"`
6. `tail = node` — new tail → `kFound`
7. Final snapshot

**InsertAt(index, val)** — special-case index 0 → prepend steps; index == length → append steps. Middle case:
1. Initial snapshot
2. Traverse `curr` from head to `index - 1`, one `kActive` step per node
3. New node created (`kNew`)
4. `node->next = curr->next`
5. `curr->next = node` — curr's `has_next` rewired, new node inserted, state → `kTarget`
6. Final snapshot

**RemoveAt(index)** — special-case index 0: head = head->next, no traversal needed.
1. Initial snapshot
2. Traverse to `index - 1`, curr → `kActive` each step
3. Node at `index` highlighted → `kRemoved`; `trace_entry`: `"found node to remove"`
4. `prev->next = curr->next` — prev's `has_next` jumps over removed node
5. Snapshot with removed node still shown (state `kRemoved`, `has_next = false`) — represents `delete tmp`
6. Final snapshot — node gone from `nodes` vector

**Get(index)**
1. Initial snapshot, curr at head
2. Traverse step by step, curr → `kActive` per step
3. At target index: node → `kFound`
4. Final snapshot with found node highlighted

---

## 4. Pseudocode — `src/viz/list_code_panel.h`

One `CodePanel` per operation (10 total). Line numbers must align with the
`current_line` values assigned in the recorder. Same pattern as `code_panel.h`.

Example — `singly_prepend_code()`:

```
lines = {
    "Node* node = new Node(val);",    // line 0
    "node->next = head;",             // line 1
    "head = node;",                   // line 2
    "++length;",                      // line 3
}
```

Example — `singly_insert_at_code()`:

```
lines = {
    "if (idx == 0) { prepend(val); return; }",     // line 0
    "if (idx == length) { append(val); return; }",  // line 1
    "Node* curr = head;",                           // line 2
    "for (size_t i = 0; i < idx-1; ++i) {",        // line 3
    "    curr = curr->next;",                       // line 4
    "}",                                            // line 5
    "Node* node = new Node(val);",                  // line 6
    "node->next = curr->next;",                     // line 7
    "curr->next = node;",                           // line 8
    "++length;",                                    // line 9
}
```

A corresponding `singly_insert_at_line(phase)` function maps a `Phase` enum
(or equivalent state booleans) to the active line number, identical to how
`binary_search_line(found, val_lt, finished)` works today.

---

## 5. Config Overlay — `src/viz/list_config.h`

The existing `ConfigPanel` is array-specific (size, distribution, optional target).
Linked list config has a different parameter set. To avoid polluting `config_overlay.h`,
a dedicated `ListConfigPanel` is introduced.

### `ListConfig` (input to recorder)

```cpp
struct ListConfig {
  std::vector<int> initial_values;
  ListOp           op    = ListOp::kAppend;
  int              value = 0;     // used by Prepend, Append, InsertAt
  int              index = 0;     // used by InsertAt, RemoveAt, Get
};
```

### `ListConfigResult` (output of overlay)

```cpp
struct ListConfigResult {
  ListConfig  config;
  std::string test_case_label;
  bool        applied = false;
};
```

### `ListConfigPanel` UI layout

```
 Configure: singly_linked_list
──────────────────────────────
 [ Dataset ]  [ Test Cases ]
──────────────────────────────
 Operation           Value / Index
 ○ Prepend           Value: [___]
 ○ Append
 ○ Insert At         Index: [___]
 ○ Remove At
 ○ Get
──────────────────────────────
 [Enter] Apply    [Esc/C] Close
```

When `op` is `kPrepend` or `kAppend`, only the Value input is shown.
When `op` is `kInsertAt`, both are shown.
When `op` is `kRemoveAt` or `kGet`, only Index is shown.

The **Test Cases** tab mirrors the existing pattern: a `Radiobox` listing
pre-built scenarios (see §7 below). Selecting one sets `initial_values`, `op`,
`value`, and `index` together.

---

## 6. Renderer — `src/viz/list_viz.h`

Renders `ListAlgorithmRecording` using FTXUI. Reuses `VizController`, `CodePanel`,
`state_panel`, `trace_panel`, and the tick-thread pattern from `array_viz.h`.

### Node rendering

Each node is drawn as a labelled box. State → color mapping:

| State      | Color  |
|------------|--------|
| kNormal    | White  |
| kActive    | Yellow |
| kNew       | Cyan   |
| kTarget    | Cyan   |
| kFound     | Green  |
| kRemoved   | Red    |

Arrows between nodes:
- Singly: ` ──> `
- Doubly: ` <-> `
- Null terminator: ` ──> NULL`

Layout (rendered top-to-bottom as one wide `hbox`):

```
HEAD
 ↓
┌───┐       ┌───┐       ┌───┐       ┌───┐
│ 3 │ ──>  │ 7 │ ──>  │12 │ ──>  │ 5 │ ──> NULL
└───┘       └───┘       └───┘       └───┘
              ↑
             curr
```

The `curr` label is rendered as a second `hbox` row beneath the node row, aligned
to the position of the active node.

A new node that has not yet been linked is rendered to the far left (for prepend)
or far right (for append/insertAt) with a dashed or dim arrow stub until the
pointer-rewire step fires.

### `run_list_visualizer()` signature

```cpp
using ListRecorderFunc =
    std::function<ListAlgorithmRecording(const ListConfig&)>;

inline void run_list_visualizer(ListAlgorithmRecording   recording,
                                const CodePanel&         code,
                                ListConfig               current_config,
                                ListRecorderFunc         re_record);
```

When `apply_config` fires from `ListConfigPanel`, `re_record` is called with the
new `ListConfig`, replacing both `recording` and `code` (the new recording carries
the updated `op` which selects the correct code panel in the caller).

### Entry points

```cpp
inline void run_singly_linked_list_viz(std::vector<int> initial);
inline void run_doubly_linked_list_viz(std::vector<int> initial);
```

Both start with a default config (`op = kAppend`, `value = 99`, `index = 0`) and
call `run_list_visualizer()`. The `ListRecorderFunc` dispatches on `config.op` to
the appropriate `record_singly_*` or `record_doubly_*` function.

---

## 7. Test Cases — extend `src/viz/test_cases.h`

Add `singly_linked_list_test_cases()` and `doubly_linked_list_test_cases()`.

Because the `TestCase` struct encodes `data` + `target` (int), linked list test
cases need to extend it — or the cleaner approach is to add a separate
`ListTestCase` struct alongside the existing `TestCase`:

```cpp
struct ListTestCase {
  std::string      label;
  std::string      description;
  std::vector<int> initial_values;
  ListOp           op;
  int              value = 0;
  int              index = 0;
};
```

Example scenarios for singly:

| Label                     | Initial list    | Op         | Params    |
|---------------------------|-----------------|------------|-----------|
| Prepend to non-empty list | [3, 7, 12, 5]   | Prepend    | val=99    |
| Append to non-empty list  | [3, 7, 12]      | Append     | val=99    |
| Insert into middle        | [3, 7, 12, 5, 9]| InsertAt   | idx=2, val=99 |
| Insert at head (idx=0)    | [3, 7, 12]      | InsertAt   | idx=0, val=99 |
| Remove from middle        | [3, 7, 12, 5, 9]| RemoveAt   | idx=2     |
| Remove head               | [3, 7, 12]      | RemoveAt   | idx=0     |
| Get middle element        | [3, 7, 12, 5, 9]| Get        | idx=3     |
| Prepend to empty list     | []              | Prepend    | val=42    |

---

## 8. Tests — `tests/`

### `test_singly_linked_list.cpp`

Links against `dsa` library. Tests each method with the same edge-case pattern
used for array algorithms. Representative cases:

- `Prepend.ToEmpty`: length becomes 1, get(0) returns value
- `Prepend.ToNonEmpty`: new head, old head now at index 1
- `Append.ToEmpty`: same as prepend on empty
- `Append.ToNonEmpty`: tail becomes new value
- `InsertAt.Middle`: element visible at target index, neighbours unchanged
- `InsertAt.IndexZero`: equivalent to prepend
- `InsertAt.IndexLength`: equivalent to append
- `RemoveAt.Middle`: length decreases by 1, element gone
- `RemoveAt.Head`: head advances to next
- `Get.ValidIndex`: returns correct value
- `Get.OutOfBounds`: throws or returns sentinel (match whatever the stub documents)

### `test_doubly_linked_list.cpp`

Same structure. Add:
- `InsertAt.CheckPrevPointers`: node at idx-1 and idx+1 have correct `prev`/`next`
- `RemoveAt.CheckPrevPointers`: neighbours re-linked bidirectionally

### `test_list_viz_recordings.cpp`

Links against `viz` library. Verifies recorder correctness:

```cpp
TEST(ListRecordings, SinglyAppendHasCorrectFinalState) {
  auto rec = viz::record_singly_append({3, 7, 12}, 99);
  ASSERT_FALSE(rec.steps.empty());
  EXPECT_EQ(rec.algorithm_name, "singly_linked_list");
  EXPECT_EQ(rec.op, viz::ListOp::kAppend);
  // Last snapshot should contain 4 nodes, last is kFound
  auto& last = rec.steps.back();
  ASSERT_EQ(last.nodes.size(), 4u);
  EXPECT_EQ(last.nodes.back().state, viz::ListNodeState::kFound);
  EXPECT_EQ(last.nodes.back().value, 99);
}
```

---

## 9. Menu Wiring — `apps/main.cpp`

```cpp
constexpr int kSinglyLinkedList = 4;
constexpr int kDoublyLinkedList = 5;

// In build_menu_entries():
{.label = "  Singly Linked List", .is_category = false, .algo_id = kSinglyLinkedList},
{.label = "  Doubly Linked List", .is_category = false, .algo_id = kDoublyLinkedList},

// In run_selected():
case kSinglyLinkedList:
  viz::run_singly_linked_list_viz({3, 7, 12, 5, 9});
  break;
case kDoublyLinkedList:
  viz::run_doubly_linked_list_viz({3, 7, 12, 5, 9});
  break;
```

---

## 10. `tests/CMakeLists.txt` changes

```cmake
target_sources(dsa_learner_tests PRIVATE
    test_singly_linked_list.cpp
    test_doubly_linked_list.cpp)

target_sources(dsa_tests PRIVATE
    test_list_viz_recordings.cpp)
```

---

## Implementation Order

Work through the layers bottom-up so each is testable before the next depends on it:

1. `list_snapshot.h` — types only, no logic
2. `singly_linked_list.h` stub + `test_singly_linked_list.cpp` (TDD — stubs fail first)
3. `doubly_linked_list.h` stub + `test_doubly_linked_list.cpp`
4. `list_code_panel.h` — pseudocode strings only
5. `list_recorder.h` (singly operations) + `test_list_viz_recordings.cpp`
6. `list_recorder.h` (doubly operations)
7. `list_config.h`
8. `list_viz.h` — renderer + entry points
9. `apps/main.cpp` + `tests/CMakeLists.txt` wiring
