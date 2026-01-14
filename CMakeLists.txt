cmake_minimum_required(VERSION 3.16)
project(SentinelOrchestra LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =========================
# Sim Core (sibling repo)
# =========================
add_subdirectory(../Sentinel-sim-core- simcore_build)

# =========================
# Orchestra Core Library
# =========================
add_library(sentinel-orchestra-core STATIC
    src/orchestra.cpp
    src/simcore_system.cpp
)

target_include_directories(sentinel-orchestra-core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(sentinel-orchestra-core PUBLIC
    simcore
)

# =========================
# Orchestra Demo Executable
# =========================
add_executable(sentinel-orchestra
    src/main.cpp
)

target_link_libraries(sentinel-orchestra PRIVATE
    sentinel-orchestra-core)
