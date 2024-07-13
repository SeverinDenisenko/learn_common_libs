# Copyright 2024 Severin Denisenko

add_executable(interval_tree interval_tree.cpp)
target_link_libraries(interval_tree ${Boost_LIBRARIES} ${Fmt_LIBRARIES})
