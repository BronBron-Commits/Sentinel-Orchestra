cmake_minimum_required(VERSION 3.16)
project(SentinelOrchestra LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Orchestra core library
add_library(sentinel-orchestra-core STATIC
    src/orchestra.cpp
    src/simcore_system.cpp
)

target_include_directories(sentinel-orchestra-core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# simcore target is provided by the parent project
target_link_libraries(sentinel-orchestra-core PUBLIC
    simcore
)
