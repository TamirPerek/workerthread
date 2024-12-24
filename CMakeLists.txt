cmake_minimum_required(VERSION 3.10.0)

project(ThreadPool VERSION 0.1.0)

include(CTest)
include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckTypeSize)
include(CheckFunctionExists)
include(CheckCXXSourceCompiles)
include(CheckCXXCompilerFlag)
include(CheckCCompilerFlag)

option(THREADPOOL_INCLUDE_TESTS "Remove GoogleTest and Signals dependecy." ON)

if(NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(THREADPOOL_INCLUDE_TESTS OFF)   
endif()

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(lib)

include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

file(GLOB TARGET_SRC
    PRIVATE
    "src/ThreadPool.cpp"
    PUBLIC
    "include/ThreadPool.hpp"
)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME} PRIVATE 
    src/ThreadPool.cpp
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ThreadPool.hpp
)
target_include_directories(${PROJECT_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

include(./cmake/CompilerWarnings.cmake)
set_project_warnings(${PROJECT_NAME})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_NAME_VERSION})
include(CPack)

if(THREADPOOL_INCLUDE_TESTS)
    include(GoogleTest)
    add_subdirectory(test) 
endif()
