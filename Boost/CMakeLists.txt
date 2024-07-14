# Copyright 2024 Severin Denisenko

add_executable(interval_tree interval_tree.cpp)
target_link_libraries(interval_tree ${Boost_LIBRARIES} ${Fmt_LIBRARIES})

add_executable(lru_set lru_set.cpp)
target_link_libraries(lru_set ${Boost_LIBRARIES} ${Fmt_LIBRARIES})

add_executable(lru_map lru_map.cpp)
target_link_libraries(lru_map ${Boost_LIBRARIES} ${Fmt_LIBRARIES})
