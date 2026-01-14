cmake_minimum_required(VERSION 3.10)
project(SentinelOrchestra LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==============================
# Bring in sim-core (NO tests)
# ==============================
add_subdirectory(
    ../Sentinel-sim-core-
    simcore_build
    EXCLUDE_FROM_ALL
)

add_executable(sentinel-orchestra
    src/main.cpp
    src/orchestra.cpp
    src/simcore_system.cpp
)

target_include_directories(sentinel-orchestra
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(sentinel-orchestra
    PRIVATE
        simcore
)
