cmake_minimum_required(VERSION 3.2.0)

set(This WorkerThread)

project(${This} VERSION 0.1.0)

option(WORKERTHREAD_INCLUDE_TESTS "Remove GoogleTest and Signals dependecy." ON)

enable_testing()

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(src)
add_subdirectory(lib)

set(CPACK_PROJECT_NAME ${THIS})
set(CPACK_PROJECT_VERSION ${THIS})
include(CPack)

if(WORKERTHREAD_INCLUDE_TESTS)
    add_subdirectory(test) 
endif()