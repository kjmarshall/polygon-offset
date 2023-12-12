cmake_minimum_required(VERSION 3.15.0)

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

set_property(GLOBAL PROPERTY USE_FOLDERS On)

message(STATUS "CMake version is ${CMAKE_VERSION}")

project(Diverget3D-Offsetting)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CONFIGURATION_TYPES Debug Release MinSizeRel RelWithDebInfo)

add_subdirectory(external/json)
include_directories(external/json/include)
add_executable(offset src/offset.cpp)
