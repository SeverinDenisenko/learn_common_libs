# Copyright 2024 Severin Denisenko

add_executable(dynamic_loading dynamic_loading.cpp)
target_link_libraries(dynamic_loading ${Folly_LIBRARIES} ${Boost_LIBRARIES} ${Raylib_LIBRARIES})
