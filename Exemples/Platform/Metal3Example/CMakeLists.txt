# CMake file for Metal 3 Platform example target


# Setup Target
if (nebula_cmake_platform STREQUAL MacOS)
    add_executable(PlatformMetal3Example MACOSX_BUNDLE source/PlatformApp.cpp)
else ()
    add_executable(PlatformMetal3Example source/PlatformApp.cpp)
endif ()


# Output Directory
set_target_properties(PlatformMetal3Example PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/PlatformMetal3Example")
target_link_libraries(PlatformMetal3Example PRIVATE Nebula-Platform)


target_include_directories(PlatformMetal3Example PRIVATE
        ../../../NebulaEngine/source
        ../../../NebulaEngine/vendor/spdlog/include
        $ENV{WINDOWS_SDK_INCLUDE_DIR}/um
        ../../../NebulaEngine/vendor/Glad/include
        ../../../NebulaEngine/vendor/metal-cpp
)
target_include_directories(PlatformMetal3Example PRIVATE $ENV{GENERATED_DIR})


# Compile Options
if (nebula_cmake_platform STREQUAL MacOS)
    add_custom_target(PlatformMetal3ExampleCopyPlatformMetal3ExampleAssets
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets/" "$<TARGET_BUNDLE_CONTENT_DIR:PlatformMetal3Example>/assets/PlatformMetal3Example"
            COMMENT "Copying PlatformMetal3Example assets to build directory..."
    )
else ()
    add_custom_target(PlatformMetal3ExampleCopyPlatformMetal3ExampleAssets
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets/" "$<TARGET_FILE_DIR:PlatformMetal3Example>/assets/PlatformMetal3Example"
            COMMENT "Copying PlatformMetal3Example assets to build directory..."
    )
endif ()
add_dependencies(PlatformMetal3Example PlatformMetal3ExampleCopyPlatformMetal3ExampleAssets)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET PlatformMetal3Example PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
    target_compile_definitions(PlatformMetal3Example PRIVATE _DEBUG)
else ()
    set_property(TARGET PlatformMetal3Example PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif ()
target_compile_definitions(PlatformMetal3Example PUBLIC NB_PLATFORM_APP_NAME="PlatformMetal3Example")


if (nebula_cmake_platform STREQUAL Windows)
    add_custom_command(
            TARGET PlatformMetal3Example
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy "${CMAKE_BINARY_DIR}/generated/deps/Microsoft.WindowsAppSDK.Foundation.2.0.21/runtimes/win-x64/native/Microsoft.WindowsAppRuntime.Bootstrap.dll" "$<TARGET_FILE_DIR:PlatformMetal3Example>/Microsoft.WindowsAppRuntime.Bootstrap.dll"
    )

elseif (nebula_cmake_platform STREQUAL MacOS)
    set_target_properties(PlatformMetal3Example PROPERTIES
                          MACOSX_BUNDLE TRUE
                          MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Platform/MacOS/Bundle/MacOSXBundleInfo.plist.in"

                          MACOSX_BUNDLE_BUNDLE_NAME "Platform Exemple"
                          MACOSX_BUNDLE_GUI_IDENTIFIER "com.nebula.PlatformMetal3Example"
                          XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.nebula.PlatformMetal3Example"
                          MACOSX_BUNDLE_BUNDLE_VERSION "1"
                          MACOSX_BUNDLE_SHORT_VERSION_STRING "dev"
                          MACOSX_BUNDLE_COPYRIGHT "© 2024 - 2026 Infinity"
    )
endif ()
