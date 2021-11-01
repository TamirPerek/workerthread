file(GLOB_RECURSE TARGET_SRC "./*.cpp" "./*.hpp" "./*.h")

add_library(${This} STATIC ${TARGET_SRC})

target_link_libraries (${This} PUBLIC signals)