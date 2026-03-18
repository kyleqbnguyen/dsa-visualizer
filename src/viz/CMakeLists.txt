add_library(viz INTERFACE)
target_include_directories(viz INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(viz INTERFACE dsa ftxui::component ftxui::dom
                                    ftxui::screen)
