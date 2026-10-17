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
target_compile_definitions(Nebula-Platform PUBLIC VK_NO_PROTOTYPES)
target_compile_definitions(Nebula-Platform PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)


# Libraries
target_include_directories(Nebula-Platform PRIVATE
                           ../
                           ../../vendor/spdlog/include
                           ../../vendor/Glad/include
                           ../../vendor/metal-cpp
                           ../../vendor/volk
                           ../../vendor/Vulkan-Headers/include
                           ${CMAKE_CURRENT_SOURCE_DIR}/swift
)


# Windows Settings
if (nebula_cmake_platform STREQUAL Windows)
    target_compile_definitions(Nebula-Platform PUBLIC NB_PLATFORM_WINDOWS)

    target_link_libraries(Nebula-Platform PRIVATE
                          GLAD
                          GLAD_WGL
                          OneCoreUAP
                          opengl32
                          volk
                          WindowsApp
    )

    add_dependencies(Nebula-Platform generate_winrt)

    target_include_directories(Nebula-Platform PRIVATE
                               $ENV{GENERATED_DIR}
                               "${CMAKE_BINARY_DIR}/generated/deps/Microsoft.WindowsAppSDK.Runtime.2.1.3/include"
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
                          GLAD
                          volk
                          ${EGL_LIBRARIES}
    )

    target_include_directories(Nebula-Platform PUBLIC
                               ../../vendor/SDL/include
                               ${EGL_INCLUDE_DIRS}
    )

elseif (nebula_cmake_platform STREQUAL MacOS)
    target_compile_definitions(Nebula-Platform PUBLIC NB_PLATFORM_MACOS)
    target_compile_options(Nebula-Platform PUBLIC "$<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default>")

    # MacOS Window
    add_library(Swift-MacOS-Window STATIC)
    target_sources(Swift-MacOS-Window PRIVATE "Platform/MacOS/Windows/MacOSWindow.swift")
    set_target_properties(Swift-MacOS-Window PROPERTIES
                          Swift_MODULE_NAME "SwiftMacOSWindow"
                          XCODE_ATTRIBUTE_PRODUCT_MODULE_NAME "SwiftMacOSWindow"
    )
    target_compile_options(Swift-MacOS-Window PUBLIC "$<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default>")
    _swift_generate_cxx_header(Swift-MacOS-Window
            "${CMAKE_CURRENT_SOURCE_DIR}/swift/SwiftMacOSWindow.h"
            SEARCH_PATHS
            "${CMAKE_CURRENT_SOURCE_DIR}/swift"
            "${CMAKE_SOURCE_DIR}/NebulaEngine/source"
            "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform"
            "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Nebula/CoreSystems/Windows"
            "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Platform/MacOS/Rendering"
            "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Platform/MacOS/Events"
            "${CMAKE_SOURCE_DIR}/NebulaEngine/vendor/metal-cpp"
    )
    target_include_directories(Swift-MacOS-Window PUBLIC
            ../
            ../../vendor/spdlog/include
            ../../vendor/Glad/include
            "${CMAKE_CURRENT_SOURCE_DIR}/swift"
            "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Nebula/CoreSystems/Windows"
            "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Platform/MacOS/Rendering"
            "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Platform/MacOS/Events"
            "${CMAKE_SOURCE_DIR}/NebulaEngine/vendor/metal-cpp"
    )

    # Metal3 Layer
    add_library(Swift-Metal3-Surface STATIC)
    target_sources(Swift-Metal3-Surface PRIVATE "Platform/MacOS/Rendering/Metal3Surface.swift")
    set_target_properties(Swift-Metal3-Surface PROPERTIES
            Swift_MODULE_NAME "SwiftMetal3Surface"
            XCODE_ATTRIBUTE_PRODUCT_MODULE_NAME "SwiftMetal3Surface"
    )
    target_compile_options(Swift-Metal3-Surface PUBLIC "$<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default>")
    _swift_generate_cxx_header(Swift-Metal3-Surface
            "${CMAKE_CURRENT_SOURCE_DIR}/swift/SwiftMetal3Surface.h"
            SEARCH_PATHS
            "${CMAKE_CURRENT_SOURCE_DIR}/swift"
            "${CMAKE_SOURCE_DIR}/NebulaEngine/source"
            "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform"
            "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Nebula/CoreSystems/Windows"
            "${CMAKE_SOURCE_DIR}/NebulaEngine/vendor/metal-cpp"
    )
    target_include_directories(Swift-Metal3-Surface PUBLIC
            ../
            ../../vendor/spdlog/include
            ../../vendor/Glad/include
            ../../vendor/metal-cpp
            "${CMAKE_CURRENT_SOURCE_DIR}/swift"
            "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Nebula/CoreSystems/Windows"
    )

    # CoreUtils
    add_library(Swift-CoreUtils STATIC)
    target_sources(Swift-CoreUtils PRIVATE "Platform/MacOS/Core/MacOSCoreUtils.swift")
    set_target_properties(Swift-CoreUtils PROPERTIES
                          Swift_MODULE_NAME "SwiftCoreUtils"
                          XCODE_ATTRIBUTE_PRODUCT_MODULE_NAME "SwiftCoreUtils"
    )
    target_compile_options(Swift-CoreUtils PUBLIC "$<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default>")
    _swift_generate_cxx_header(Swift-CoreUtils
                               "${CMAKE_CURRENT_SOURCE_DIR}/swift/SwiftCoreUtils.h"
                               SEARCH_PATHS
                               "${CMAKE_CURRENT_SOURCE_DIR}/swift"
                               "${CMAKE_SOURCE_DIR}/NebulaEngine/source"
                               "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform"
    )
    target_include_directories(Swift-CoreUtils PUBLIC
                               ../
                               "${CMAKE_CURRENT_SOURCE_DIR}/swift"
    )

    # PlatformUtils
    add_library(Swift-PlatformUtils STATIC)
    target_sources(Swift-PlatformUtils PRIVATE "Platform/MacOS/Core/MacOSPlatformUtils.swift")
    set_target_properties(Swift-PlatformUtils PROPERTIES
                          Swift_MODULE_NAME "SwiftPlatformUtils"
                          XCODE_ATTRIBUTE_PRODUCT_MODULE_NAME "SwiftPlatformUtils"
    )
    target_compile_options(Swift-PlatformUtils PUBLIC "$<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default>")
    _swift_generate_cxx_header(Swift-PlatformUtils
                               "${CMAKE_CURRENT_SOURCE_DIR}/swift/SwiftPlatformUtils.h"
                               SEARCH_PATHS
                               "${CMAKE_CURRENT_SOURCE_DIR}/swift"
                               "${CMAKE_SOURCE_DIR}/NebulaEngine/source"
                               "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform"
                               "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Nebula/CoreSystems/Core"
                               "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Nebula/CoreSystems/Input"
    )
    target_include_directories(Swift-PlatformUtils PUBLIC
                               ../
                               "${CMAKE_CURRENT_SOURCE_DIR}/swift"
                               "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Nebula/CoreSystems/Core"
                               "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Nebula/CoreSystems/Input"
    )

    # File Dialogs
    add_library(Swift-FileDialogs STATIC)
    target_sources(Swift-FileDialogs PRIVATE "Platform/MacOS/Dialogs/MacOSFileDialogs.swift")
    set_target_properties(Swift-FileDialogs PROPERTIES
                          Swift_MODULE_NAME "SwiftFileDialogs"
                          XCODE_ATTRIBUTE_PRODUCT_MODULE_NAME "SwiftFileDialogs"
    )
    target_compile_options(Swift-FileDialogs PUBLIC "$<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default>")
    _swift_generate_cxx_header(Swift-FileDialogs
                               "${CMAKE_CURRENT_SOURCE_DIR}/swift/SwiftFileDialogs.h"
                               SEARCH_PATHS
                               "${CMAKE_CURRENT_SOURCE_DIR}/swift"
                               "${CMAKE_SOURCE_DIR}/NebulaEngine/source"
                               "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform"
                               "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Nebula/Dialogs"
    )
    target_include_directories(Swift-FileDialogs PUBLIC
                               ../
                               "${CMAKE_CURRENT_SOURCE_DIR}/swift"
                               "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Nebula/Dialogs"
    )

    # Input
    add_library(Swift-Input STATIC)
    target_sources(Swift-Input PRIVATE "Platform/MacOS/Input/MacOSInput.swift")
    set_target_properties(Swift-Input PROPERTIES
                          Swift_MODULE_NAME "SwiftInput"
                          XCODE_ATTRIBUTE_PRODUCT_MODULE_NAME "SwiftInput"
    )
    target_compile_options(Swift-Input PUBLIC "$<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default>")
    _swift_generate_cxx_header(Swift-Input
                               "${CMAKE_CURRENT_SOURCE_DIR}/swift/SwiftInput.h"
                               SEARCH_PATHS
                               "${CMAKE_CURRENT_SOURCE_DIR}/swift"
                               "${CMAKE_SOURCE_DIR}/NebulaEngine/source"
                               "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform"
                               "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Nebula/CoreSystems/Input"
    )
    target_include_directories(Swift-Input PUBLIC
                               ../
                               "${CMAKE_CURRENT_SOURCE_DIR}/swift"
                               "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Nebula/CoreSystems/Input"
    )

    target_link_libraries(Nebula-Platform PUBLIC
            Swift-MacOS-Window
            Swift-Metal3-Surface
            Swift-CoreUtils
            Swift-PlatformUtils
            Swift-FileDialogs
            Swift-Input
    )
endif ()