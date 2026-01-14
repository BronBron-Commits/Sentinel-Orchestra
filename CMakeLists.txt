cmake_minimum_required(VERSION 3.10)
project(SentinelOrchestra LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sentinel-orchestra src/main.cpp src/orchestra.cpp)
