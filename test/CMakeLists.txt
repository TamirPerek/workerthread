
include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

project(WorkerThreadTests)

enable_testing()

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(${PROJECT_NAME}
    ThreadPoolTest.cpp
)

target_link_libraries(${PROJECT_NAME}
                        gtest_main
                        ThreadPool
)

gtest_discover_tests(${PROJECT_NAME})
