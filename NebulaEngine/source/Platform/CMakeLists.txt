# Cmake file for Nebula Engine - Platform target


# Standard Source Files
file(GLOB_RECURSE NebulaPlatformCoreSourceFiles
     "Nebula/*.cpp"
)

file(GLOB_RECURSE NebulaPlatformCoreHeaderFile
     "Nebula/*.h"
     "Nebula/*.hpp"
)


if (nebula_cmake_platform STREQUAL Windows)
    file(GLOB_RECURSE NebulaPlatformOSSourceFiles
         "Platform/Windows/*.cpp"
    )
    file(GLOB_RECURSE NebulaPlatformOSHeaderFiles
         "Platform/Windows/*.h"
         "Platform/Windows/*.hpp"
    )
elseif (nebula_cmake_platform STREQUAL Linux)
    file(GLOB_RECURSE NebulaPlatformOSSourceFiles
         "Platform/Linux/*.cpp"
    )
    file(GLOB_RECURSE NebulaPlatformOSHeaderFiles
         "Platform/Linux/*.h"
         "Platform/Linux/*.hpp"
    )
elseif (nebula_cmake_platform STREQUAL MacOS)
    file(GLOB_RECURSE NebulaPlatformOSSourceFiles
         "Platform/MacOS/*.cpp"
    )
    file(GLOB_RECURSE NebulaPlatformOSHeaderFiles
         "Platform/MacOS/*.h"
         "Platform/MacOS/*.hpp"
    )
    list(APPEND NebulaPlatformCoreSourceFiles ${NebulaPlatformOSSourceFiles})
    list(APPEND NebulaPlatformCoreHeaderFiles ${NebulaPlatformOSHeaderFiles})
elseif (nebula_cmake_platform STREQUAL iOS)
    file(GLOB_RECURSE NebulaPlatformOSSourceFiles
         "Platform/iOS/*.cpp"
    )
    file(GLOB_RECURSE NebulaPlatformOSHeaderFiles
         "Platform/iOS/*.h"
         "Platform/iOS/*.hpp"
    )
    list(APPEND NebulaPlatformCoreSourceFiles ${NebulaPlatformOSSourceFiles})
    list(APPEND NebulaPlatformCoreHeaderFiles ${NebulaPlatformOSHeaderFiles})
endif ()
list(APPEND NebulaPlatformCoreSourceFiles ${NebulaPlatformOSSourceFiles})
list(APPEND NebulaPlatformCoreHeaderFiles ${NebulaPlatformOSHeaderFiles})


# Setup Target
add_library(Nebula-Platform STATIC)
target_sources(Nebula-Platform PRIVATE ${NebulaPlatformCoreSourceFiles} PUBLIC ${NebulaPlatformCoreHeaderFiles})


# Output Directory
set_target_properties(Nebula-Platform PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/NebulaEngine")


# Compilation Options
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET Nebula-Platform PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
else ()
    set_property(TARGET Nebula-Platform PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif ()


# Defines
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Nebula-Platform PUBLIC NB_ENGINE_DEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(Nebula-Platform PUBLIC NB_RELEASE)
else ()
    target_compile_definitions(Nebula-Platform PUBLIC NB_DIST)
endif ()


# Libraries
target_link_libraries(Nebula-Platform PRIVATE
    GLAD
)
target_include_directories(Nebula-Platform PRIVATE
    ../
    ../../vendor/spdlog/include
    ../../vendor/Glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/swift
)


# Windows Settings
if (nebula_cmake_platform STREQUAL Windows)
    target_compile_definitions(Nebula-Platform PUBLIC NB_PLATFORM_WINDOWS)

    target_link_libraries(Nebula-Platform PRIVATE
        WindowsApp
        OneCoreUAP
        GLAD_WGL
        opengl32
    )

    add_dependencies(Nebula-Platform generate_winrt)

    target_include_directories(Nebula-Platform PRIVATE
                               $ENV{GENERATED_DIR}
                               "${CMAKE_SOURCE_DIR}/NebulaEngine/vendor/WindowsAppSDK/dev/WindowsAppRuntime_BootstrapDLL"
                               "$ENV{WINDOWS_SDK_INCLUDE_DIR}/um"
    )

    # Force WinRT debugging support
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(Nebula-Platform PRIVATE -o0)
        target_compile_definitions(Nebula-Platform PRIVATE _DEBUG WINRT_NO_MAKE_DETECTION)
    endif()

elseif (nebula_cmake_platform STREQUAL Linux)
    target_compile_definitions(Nebula-Platform PUBLIC NB_PLATFORM_LINUX)

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(EGL REQUIRED egl)

    target_link_libraries(Nebula-Platform PRIVATE
                          SDL3::SDL3
                          ${EGL_LIBRARIES}
    )

    target_include_directories(Nebula-Platform PUBLIC
                               ../../vendor/SDL/include
                               ${EGL_INCLUDE_DIRS}
    )

elseif (nebula_cmake_platform STREQUAL MacOS)
    target_compile_definitions(Nebula-Platform PUBLIC NB_PLATFORM_MACOS)
    target_compile_options(Nebula-Platform PUBLIC "$<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default>")

    add_subdirectory(Platform/MacOS)

    target_link_libraries(Nebula-Platform PUBLIC
                          Swift-MacOS-Window
                          Swift-OpenGL-Context
                          Swift-MacOS-CoreUtils
                          Swift-PlatformUtils
                          Swift-FileDialogs
                          Swift-Input
    )
    target_include_directories(Nebula-Platform PUBLIC
                               Platform/MacOS/swift
    )

elseif (nebula_cmake_platform STREQUAL iOS)
    target_compile_definitions(Nebula-Platform PUBLIC NB_PLATFORM_IOS)
    target_compile_options(Nebula-Platform PUBLIC "$<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default>")

    add_subdirectory(Platform/iOS)

    target_link_libraries(Nebula-Platform PUBLIC
                          Swift-iOS-Window
    )
    target_include_directories(Nebula-Platform PUBLIC
                               Platform/iOS/swift
    )
endif ()