# Copyright 2024 Severin Denisenko

cmake_minimum_required(VERSION 3.28)
project(learn_folly)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(folly REQUIRED)
set(Folly_LIBRARIES Folly::folly)
find_package(Boost REQUIRED)
find_package(gflags REQUIRED)
find_package(fmt REQUIRED)
find_package(Qt6 COMPONENTS Core Widgets OpenGL Gui Concurrent REQUIRED)
set(Qt_LIBRARIES Qt6::Core Qt6::Widgets Qt6::OpenGL Qt6::Concurrent)
set(Fmt_LIBRARIES fmt::fmt)

add_subdirectory(Boost)
add_subdirectory(Folly)
add_subdirectory(Qt)
