cmake_minimum_required(VERSION 3.1.0)
project(rootname)

set(CMAKE_C_STANDARD 99)

include(FindPkgConfig)
pkg_check_modules(XCB REQUIRED xcb)

add_executable(rootname src/rootname.c)
target_link_libraries(rootname "${XCB_LIBRARIES}")
