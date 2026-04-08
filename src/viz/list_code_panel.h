#pragma once

#include "code_panel.h"

namespace viz {

// --- Singly Linked List ---

inline auto singly_prepend_code() -> CodePanel {
  return {
      .title = "singly_linked_list.h — prepend",
      .lines =
          {
              "Node* node = new Node(val);", // 0
              "node->next = head;",          // 1
              "head = node;",                // 2
              "++length;",                   // 3
          },
  };
}

inline auto singly_append_code() -> CodePanel {
  return {
      .title = "singly_linked_list.h — append",
      .lines =
          {
              "Node* node = new Node(val);",     // 0
              "Node* curr = head;",              // 1
              "while (curr->next != nullptr) {", // 2
              "    curr = curr->next;",          // 3
              "}",                               // 4
              "curr->next = node;",              // 5
              "tail = node;",                    // 6
              "++length;",                       // 7
          },
  };
}

inline auto singly_insert_at_code() -> CodePanel {
  return {
      .title = "singly_linked_list.h — insert_at",
      .lines =
          {
              "if (idx == 0) { prepend(val); return; }",     // 0
              "if (idx == length) { append(val); return; }", // 1
              "Node* curr = head;",                          // 2
              "for (size_t i = 0; i < idx-1; ++i) {",        // 3
              "    curr = curr->next;",                      // 4
              "}",                                           // 5
              "Node* node = new Node(val);",                 // 6
              "node->next = curr->next;",                    // 7
              "curr->next = node;",                          // 8
              "++length;",                                   // 9
          },
  };
}

inline auto singly_remove_at_code() -> CodePanel {
  return {
      .title = "singly_linked_list.h — remove_at",
      .lines =
          {
              "if (idx == 0) { head = head->next; return; }", // 0
              "Node* prev = head;",                           // 1
              "for (size_t i = 0; i < idx-1; ++i) {",         // 2
              "    prev = prev->next;",                       // 3
              "}",                                            // 4
              "Node* tmp = prev->next;",                      // 5
              "prev->next = tmp->next;",                      // 6
              "delete tmp;",                                  // 7
              "--length;",                                    // 8
          },
  };
}

inline auto singly_get_code() -> CodePanel {
  return {
      .title = "singly_linked_list.h — get",
      .lines =
          {
              "Node* curr = head;",                 // 0
              "for (size_t i = 0; i < idx; ++i) {", // 1
              "    curr = curr->next;",             // 2
              "}",                                  // 3
              "return curr->val;",                  // 4
          },
  };
}

// --- Doubly Linked List ---

inline auto doubly_prepend_code() -> CodePanel {
  return {
      .title = "doubly_linked_list.h — prepend",
      .lines =
          {
              "Node* node = new Node(val);",  // 0
              "node->next = head;",           // 1
              "if (head) head->prev = node;", // 2
              "head = node;",                 // 3
              "++length;",                    // 4
          },
  };
}

inline auto doubly_append_code() -> CodePanel {
  return {
      .title = "doubly_linked_list.h — append",
      .lines =
          {
              "Node* node = new Node(val);",     // 0
              "Node* curr = head;",              // 1
              "while (curr->next != nullptr) {", // 2
              "    curr = curr->next;",          // 3
              "}",                               // 4
              "node->prev = curr;",              // 5
              "curr->next = node;",              // 6
              "tail = node;",                    // 7
              "++length;",                       // 8
          },
  };
}

inline auto doubly_insert_at_code() -> CodePanel {
  return {
      .title = "doubly_linked_list.h — insert_at",
      .lines =
          {
              "if (idx == 0) { prepend(val); return; }",     // 0
              "if (idx == length) { append(val); return; }", // 1
              "Node* curr = head;",                          // 2
              "for (size_t i = 0; i < idx; ++i) {",          // 3
              "    curr = curr->next;",                      // 4
              "}",                                           // 5
              "Node* node = new Node(val);",                 // 6
              "node->next = curr;",                          // 7
              "node->prev = curr->prev;",                    // 8
              "curr->prev->next = node;",                    // 9
              "curr->prev = node;",                          // 10
              "++length;",                                   // 11
          },
  };
}

inline auto doubly_remove_at_code() -> CodePanel {
  return {
      .title = "doubly_linked_list.h — remove_at",
      .lines =
          {
              "Node* curr = head;",                             // 0
              "for (size_t i = 0; i < idx; ++i) {",             // 1
              "    curr = curr->next;",                         // 2
              "}",                                              // 3
              "if (curr->prev) curr->prev->next = curr->next;", // 4
              "if (curr->next) curr->next->prev = curr->prev;", // 5
              "delete curr;",                                   // 6
              "--length;",                                      // 7
          },
  };
}

inline auto doubly_get_code() -> CodePanel {
  return {
      .title = "doubly_linked_list.h — get",
      .lines =
          {
              "Node* curr = head;",                 // 0
              "for (size_t i = 0; i < idx; ++i) {", // 1
              "    curr = curr->next;",             // 2
              "}",                                  // 3
              "return curr->val;",                  // 4
          },
  };
}

// --- Code panel selector ---

inline auto idle_code_panel() -> CodePanel {
  return {
      .title = "No operation selected",
      .lines =
          {
              "Press [C] to configure an operation",
          },
  };
}

inline auto get_singly_code_panel(ListOp op) -> CodePanel {
  switch (op) {
  case ListOp::kPrepend:
    return singly_prepend_code();
  case ListOp::kAppend:
    return singly_append_code();
  case ListOp::kInsertAt:
    return singly_insert_at_code();
  case ListOp::kRemoveAt:
    return singly_remove_at_code();
  case ListOp::kGet:
    return singly_get_code();
  default:
    return idle_code_panel();
  }
}

inline auto get_doubly_code_panel(ListOp op) -> CodePanel {
  switch (op) {
  case ListOp::kPrepend:
    return doubly_prepend_code();
  case ListOp::kAppend:
    return doubly_append_code();
  case ListOp::kInsertAt:
    return doubly_insert_at_code();
  case ListOp::kRemoveAt:
    return doubly_remove_at_code();
  case ListOp::kGet:
    return doubly_get_code();
  default:
    return idle_code_panel();
  }
}

} // namespace viz
