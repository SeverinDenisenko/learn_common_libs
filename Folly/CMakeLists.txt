# Copyright 2024 Severin Denisenko

add_executable(thread_pool thread_pool.cpp)
target_link_libraries(thread_pool ${Folly_LIBRARIES} ${Boost_LIBRARIES})

add_executable(fibers_trivial fibers_trivial.cpp)
target_link_libraries(fibers_trivial ${Folly_LIBRARIES} ${Boost_LIBRARIES})
