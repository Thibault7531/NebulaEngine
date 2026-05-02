# iOS Window
add_library(Swift-iOS-Window STATIC)
target_sources(Swift-iOS-Window PRIVATE "Windows/iOSWindow.swift")
set_target_properties(Swift-iOS-Window PROPERTIES
                      Swift_MODULE_NAME "SwiftiOSWindow"
                      XCODE_ATTRIBUTE_PRODUCT_MODULE_NAME "SwiftiOSWindow"
)
target_compile_options(Swift-iOS-Window PUBLIC "$<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default>")
_swift_generate_cxx_header(Swift-iOS-Window
                           "${CMAKE_CURRENT_SOURCE_DIR}/swift/SwiftiOSWindow.h"
                           SEARCH_PATHS
                           "${CMAKE_CURRENT_SOURCE_DIR}/swift"
                           "${CMAKE_SOURCE_DIR}/NebulaEngine/source"
                           "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform"
                           "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Nebula/CoreSystems/Windows"
)
target_include_directories(Swift-iOS-Window PUBLIC
                           ../../../
                           ../../../../vendor/spdlog/include
                           ../../../../vendor/Glad/include
                           "${CMAKE_CURRENT_SOURCE_DIR}/swift"
                           "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Nebula/CoreSystems/Windows"
)

# iOS Core Utils
add_library(Swift-iOS-CoreUtils STATIC)
target_sources(Swift-iOS-CoreUtils PRIVATE "Core/iOSCoreUtils.swift")
set_target_properties(Swift-iOS-CoreUtils PROPERTIES
                      Swift_MODULE_NAME "SwiftiOSCoreUtils"
                      XCODE_ATTRIBUTE_PRODUCT_MODULE_NAME "SwiftiOSCoreUtils"
)
target_compile_options(Swift-iOS-CoreUtils PUBLIC "$<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default>")
_swift_generate_cxx_header(Swift-iOS-CoreUtils
                           "${CMAKE_CURRENT_SOURCE_DIR}/swift/SwiftiOSCoreUtils.h"
                           SEARCH_PATHS
                           "${CMAKE_CURRENT_SOURCE_DIR}/swift"
                           "${CMAKE_SOURCE_DIR}/NebulaEngine/source"
                           "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform"
)
target_include_directories(Swift-iOS-CoreUtils PUBLIC
                           ../../../
                           "${CMAKE_CURRENT_SOURCE_DIR}/swift"
)