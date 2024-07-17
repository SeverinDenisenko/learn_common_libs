# Copyright 2024 Severin Denisenko

cmake_minimum_required(VERSION 3.28)
project(learn_folly)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_COMPILE_COMMANDS "-Wall -Wextra -pedantic")

set(CMAKE_BUILD_TYPE "Debug")

find_package(folly REQUIRED)
set(Folly_LIBRARIES Folly::folly)
find_package(Boost REQUIRED)
set(Boost_LIBRARIES ${Boost_LIBRARIES} Boost::program_options)
find_package(gflags REQUIRED)
find_package(fmt REQUIRED)
set(Fmt_LIBRARIES fmt::fmt)
find_package(Qt6 COMPONENTS Core Widgets OpenGL Gui Concurrent REQUIRED)
set(Qt_LIBRARIES Qt6::Core Qt6::Widgets Qt6::OpenGL Qt6::Concurrent)

# Raylib from OS repos is too old
add_subdirectory(deps/raylib)
set(Raylib_LIBRARIES raylib)

add_subdirectory(Boost)
add_subdirectory(Folly)
add_subdirectory(Qt)
add_subdirectory(Raylib)
