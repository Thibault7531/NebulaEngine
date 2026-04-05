# CMake file for Nebula Project


# CMake requirement
cmake_minimum_required(VERSION 4.1.2)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/Scripts/")

# Set Apple OS SDK
if (APPLE)
    execute_process(
            COMMAND xcrun --show-sdk-path
            OUTPUT_VARIABLE SDK
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(CMAKE_OSX_DEPLOYMENT_TARGET 13.3)
    set(CMAKE_OSX_SYSROOT "${SDK}")
endif ()

# Project Specification
if (APPLE)
    project(NebulaEngine LANGUAGES CXX C Swift VERSION 0.0)
else ()
    project(NebulaEngine LANGUAGES CXX C VERSION 0.0)
endif ()
set(CMAKE_CXX_STANDARD 23)


# Multi-config Generators
set(CMAKE_CONFIGURATION_TYPES Debug Release Dist)


# Platforms
if (WIN32)
    set(nebula_cmake_platform Windows)
elseif (APPLE)
    if(CMAKE_SYSTEM_NAME MATCHES ".*(Darwin|MacOS).*")
        include(InitializeSwift)
        include(AddSwift)
        set(CMAKE_Swift_LANGUAGE_VERSION 6)
        set(nebula_cmake_platform MacOS)
    elseif(CMAKE_SYSTEM_NAME MATCHES ".*iOS.*")
        set(nebula_cmake_platform iOS)
    else()
        message(WARNING "Unknown Apple platform: \"${CMAKE_SYSTEM_NAME}\"")
    endif()
elseif (LINUX)
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(CMAKE_THREAD_LIBS_INIT "-pthread")
    set(nebula_cmake_platform Linux)
elseif (ANDROID)
    set(nebula_cmake_platform Android)
endif ()


# Compiler arguments
if (nebula_cmake_platform STREQUAL "Windows")
    set(CMAKE_C_FLAGS "-std=c11")
    set(CMAKE_C_FLAGS_DEBUG "-O0 -g -fms-runtime-lib=static_dbg")
    set(CMAKE_C_FLAGS_RELEASE "-O2 -fms-runtime-lib=static")
    set(CMAKE_C_FLAGS_DIST "-O3 -fms-runtime-lib=static")

    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -fms-runtime-lib=static_dbg")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -fms-runtime-lib=static")
    set(CMAKE_CXX_FLAGS_DIST "-O3 -fms-runtime-lib=static")
elseif (nebula_cmake_platform STREQUAL Linux)
    set(CMAKE_C_FLAGS "-std=c11")
    set(CMAKE_C_FLAGS_DEBUG "-O0 -g")
    set(CMAKE_C_FLAGS_RELEASE "-O2")
    set(CMAKE_C_FLAGS_DIST "-O3")

    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2")
    set(CMAKE_CXX_FLAGS_DIST "-O3")
elseif (nebula_cmake_platform STREQUAL MacOS)
    set(CMAKE_C_FLAGS "-std=c11")
    set(CMAKE_C_FLAGS_DEBUG "-o0 -g")
    set(CMAKE_C_FLAGS_RELEASE "-o2")
    set(CMAKE_C_FLAGS_DIST "-o3")

    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_DEBUG "-o0 -g")
    set(CMAKE_CXX_FLAGS_RELEASE "-o2")
    set(CMAKE_CXX_FLAGS_DIST "-o3")

    set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "-Wl -all_load")
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "-Wl -all_load")
    set(CMAKE_STATIC_LINKER_FLAGS_DIST "-Wl -all_load")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "-Wl -all_load")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "-Wl -all_load")
    set(CMAKE_SHARED_LINKER_FLAGS_DIST "-Wl -all_load")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-Wl -all_load")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl -all_load")
    set(CMAKE_EXE_LINKER_FLAGS_DIST "-Wl -all_load")
endif ()


# Targets
add_subdirectory(NebulaEngine)
add_subdirectory(NebulaEditor)
add_subdirectory(TestGame)
add_subdirectory(Exemples/PlatformExemple)


# Visual Studio Properties
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Nebula) #Startup Project