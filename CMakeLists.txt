cmake_minimum_required(VERSION 3.20)
project(SentinelOrchestra LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

# =============================
# simcore (source dependency)
# =============================
set(SIMCORE_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SIMCORE_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(
    ../Sentinel-sim-core-
    simcore_build
)

# =============================
# orchestra library
# =============================
add_library(orchestra
    src/orchestrator.cpp
)

# IMPORTANT: simcore must be PUBLIC
target_link_libraries(orchestra
    PUBLIC simcore
)

target_include_directories(orchestra
    PUBLIC include
)

# =============================
# orchestra tests
# =============================
add_executable(test_convergence
    tests/test_convergence.cpp
)

target_link_libraries(test_convergence
    PRIVATE orchestra
)

add_test(NAME convergence COMMAND test_convergence)

