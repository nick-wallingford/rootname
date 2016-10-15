cmake_minimum_required(VERSION 3.1.0)
project(rootname)

set(CMAKE_C_STANDARD 99)
set(SRC_LIST src/rootname.c src/util.c)

option(BATTERY "Show battery state" ON)

if(BATTERY)
  message(STATUS "With battery state")
  add_definitions(-DBATTERY)
  set(SRC_LIST ${SRC_LIST} src/power.c)
endif(BATTERY)

include(FindPkgConfig)
pkg_check_modules(XCB REQUIRED xcb)

add_executable(rootname ${SRC_LIST})
target_link_libraries(rootname "${XCB_LIBRARIES}")
