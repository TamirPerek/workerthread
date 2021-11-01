include(FetchContent)

if(WORKERTHREAD_INCLUDE_TESTS)
  # ------------- GoogleTest --------------

  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.11.0
  )
  set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
  set(BUILD_GTEST ON CACHE BOOL "" FORCE)

  FetchContent_MakeAvailable(googletest)


  # ------------- vdk::signals --------------

  FetchContent_Declare(
    signals
    GIT_REPOSITORY https://github.com/TamirPerek/signals.git
  )
  set(SIGNALS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(SIGNALS_BUILD_DEMO OFF CACHE BOOL "" FORCE)

  FetchContent_MakeAvailable(googletest signals)
endif()