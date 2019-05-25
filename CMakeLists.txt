cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project (a_star CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${A_star_SOURCE_DIR}/cmake")

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/thirdparty)
include_directories("${PROJECT_SOURCE_DIR}/src")

file(GLOB_RECURSE a_star_SOURCES "src/*.cpp")
file(GLOB_RECURSE thirdparty_SOURCES "thirdparty/*.cpp")

add_executable(astar ${a_star_SOURCES} ${thirdparty_SOURCES})
target_link_libraries(astar ${SDL2_LIBRARIES})

add_custom_command(TARGET astar PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:astar>/data)
