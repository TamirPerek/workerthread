include(FetchContent)

if(THREADPOOL_INCLUDE_TESTS)
  # ------------- GoogleTest --------------

  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
  )
  set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
  set(BUILD_GTEST ON CACHE BOOL "" FORCE)

  FetchContent_MakeAvailable(googletest)
endif()