set(ThisTest WorkerThreadTests)

file(GLOB_RECURSE TARGET_SRC "./*.cpp" "./*.hpp" "./*.h")

add_executable(${ThisTest} ${TARGET_SRC})

target_link_libraries(${ThisTest}
                        gtest_main
                        ${This}
                        signals
)

add_test(
    NAME ${ThisTest}
    COMMAND ${ThisTest}
)