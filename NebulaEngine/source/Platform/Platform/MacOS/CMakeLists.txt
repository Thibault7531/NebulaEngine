# MacOS Window
add_library(Swift-MacOS-Window STATIC)
target_sources(Swift-MacOS-Window PRIVATE "Windows/MacOSWindow.swift")
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
)
target_include_directories(Swift-MacOS-Window PUBLIC
                           ../../../
                           ../../../../vendor/spdlog/include
                           ../../../../vendor/Glad/include
                           "${CMAKE_CURRENT_SOURCE_DIR}/swift"
                           "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Nebula/CoreSystems/Windows"
                           "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Platform/MacOS/Rendering"
                           "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Platform/MacOS/Events"
)

# OpenGL Context
add_library(Swift-OpenGL-Context STATIC)
target_sources(Swift-OpenGL-Context PRIVATE "Rendering/OpenGLContext.swift")
set_target_properties(Swift-OpenGL-Context PROPERTIES
                      Swift_MODULE_NAME "SwiftOpenGLContext"
                      XCODE_ATTRIBUTE_PRODUCT_MODULE_NAME "SwiftOpenGLContext"
)
target_compile_options(Swift-OpenGL-Context PUBLIC "$<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default>")
_swift_generate_cxx_header(Swift-OpenGL-Context
                           "${CMAKE_CURRENT_SOURCE_DIR}/swift/SwiftOpenGLContext.h"
                           SEARCH_PATHS
                           "${CMAKE_CURRENT_SOURCE_DIR}/swift"
                           "${CMAKE_SOURCE_DIR}/NebulaEngine/source"
                           "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform"
                           "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Nebula/CoreSystems/Windows"
)
target_include_directories(Swift-OpenGL-Context PUBLIC
                           ../../../
                           ../../../../vendor/spdlog/include
                           ../../../../vendor/Glad/include
                           "${CMAKE_CURRENT_SOURCE_DIR}/swift"
                           "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Nebula/CoreSystems/Windows"
)

# MacOS CoreUtils
add_library(Swift-MacOS-CoreUtils STATIC)
target_sources(Swift-MacOS-CoreUtils PRIVATE "Core/MacOSCoreUtils.swift")
set_target_properties(Swift-MacOS-CoreUtils PROPERTIES
                      Swift_MODULE_NAME "SwiftMacOSCoreUtils"
                      XCODE_ATTRIBUTE_PRODUCT_MODULE_NAME "SwiftMacOSCoreUtils"
)
target_compile_options(Swift-MacOS-CoreUtils PUBLIC "$<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default>")
_swift_generate_cxx_header(Swift-MacOS-CoreUtils
                           "${CMAKE_CURRENT_SOURCE_DIR}/swift/SwiftMacOSCoreUtils.h"
                           SEARCH_PATHS
                           "${CMAKE_CURRENT_SOURCE_DIR}/swift"
                           "${CMAKE_SOURCE_DIR}/NebulaEngine/source"
                           "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform"
)
target_include_directories(Swift-MacOS-CoreUtils PUBLIC
                           ../../../
                           "${CMAKE_CURRENT_SOURCE_DIR}/swift"
)

# PlatformUtils
add_library(Swift-PlatformUtils STATIC)
target_sources(Swift-PlatformUtils PRIVATE "Core/MacOSPlatformUtils.swift")
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
                           ../../../
                           "${CMAKE_CURRENT_SOURCE_DIR}/swift"
                           "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Nebula/CoreSystems/Core"
                           "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Nebula/CoreSystems/Input"
)

# File Dialogs
add_library(Swift-FileDialogs STATIC)
target_sources(Swift-FileDialogs PRIVATE "Dialogs/MacOSFileDialogs.swift")
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
                           ../../../
                           "${CMAKE_CURRENT_SOURCE_DIR}/swift"
                           "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Nebula/Dialogs"
)

# Input
add_library(Swift-Input STATIC)
target_sources(Swift-Input PRIVATE "Input/MacOSInput.swift")
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
                           ../../../
                           "${CMAKE_CURRENT_SOURCE_DIR}/swift"
                           "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Nebula/CoreSystems/Input"
)