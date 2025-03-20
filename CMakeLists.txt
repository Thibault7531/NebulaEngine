# CMake file for Nebula Project


# CMake requirement
cmake_minimum_required(VERSION 3.30.3)


# Project Specification
project(NebulaEngine LANGUAGES CXX C VERSION 0.0)
set(CMAKE_CXX_STANDARD 23)


# Multi-config Generators
set(CMAKE_CONFIGURATION_TYPES Debug Release Dist)


# Platforms
if (WIN32)
    set(nebula_cmake_platform Windows)
elseif (APPLE)
    if(CMAKE_SYSTEM_NAME MATCHES ".*(Darwin|MacOS).*")
        set(nebula_cmake_platform MacOS)
    elseif(CMAKE_SYSTEM_NAME MATCHES ".*iOS.*")
        set(nebula_cmake_platform iOS)
    else()
        message(WARNING "Unknown Apple platform: \"${CMAKE_SYSTEM_NAME}\"")
    endif()
elseif (LINUX)
    set(nebula_cmake_platform Linux)
elseif (ANDROID)
    set(nebula_cmake_platform Android)
endif ()


# Compiler arguments
set(CMAKE_C_FLAGS "/std:c11")
set(CMAKE_C_FLAGS_DEBUG "")
set(CMAKE_C_FLAGS_RELEASE "/O2 /Ob2 /DNDEBUG")
set(CMAKE_C_FLAGS_DIST "/O2 /Ob2 /DNDEBUG")

set(CMAKE_CXX_FLAGS "/std:c++latest")
set(CMAKE_CXX_FLAGS_DEBUG "")
set(CMAKE_CXX_FLAGS_RELEASE "/O2 /Ob2 /DNDEBUG")
set(CMAKE_CXX_FLAGS_DIST "/O2 /Ob2 /DNDEBUG")

set(CMAKE_EXE_LINKER_FLAGS_DIST "/INCREMENTAL:NO")
set(CMAKE_SHARED_LINKER_FLAGS_DIST "/INCREMENTAL:NO")


# Targets
add_subdirectory(NebulaEngine)
add_subdirectory(NebulaEditor)
add_subdirectory(TestGame)


# Visual Studio Properties
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Nebula) #Startup Project