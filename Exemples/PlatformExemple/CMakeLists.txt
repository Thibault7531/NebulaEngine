# CMake file for PlatformExemple target


# Setup Target
if (nebula_cmake_platform STREQUAL MacOS)
    add_executable(PlatformExemple MACOSX_BUNDLE source/PlatformApp.cpp)
else ()
    add_executable(PlatformExemple source/PlatformApp.cpp)
endif ()


# Output Directory
set_target_properties(PlatformExemple PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/PlatformExemple")
target_link_libraries(PlatformExemple PRIVATE Nebula-Platform)


target_include_directories(PlatformExemple PRIVATE
        ../../NebulaEngine/source
        ../../NebulaEngine/vendor/spdlog/include
        $ENV{WINDOWS_SDK_INCLUDE_DIR}/um
        ../../NebulaEngine/vendor/Glad/include
)
target_include_directories(PlatformExemple PRIVATE $ENV{GENERATED_DIR})


# Compile Options
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET PlatformExemple PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
    target_compile_definitions(PlatformExemple PRIVATE _DEBUG)
else ()
    set_property(TARGET PlatformExemple PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif ()


if (nebula_cmake_platform STREQUAL Windows)
    add_custom_command(
            TARGET PlatformExemple
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy "${CMAKE_SOURCE_DIR}/NebulaEngine/vendor/winrt/Microsoft.WindowsAppRuntime.Bootstrap.dll" "$<TARGET_FILE_DIR:PlatformExemple>/Microsoft.WindowsAppRuntime.Bootstrap.dll"
    )

elseif (nebula_cmake_platform STREQUAL MacOS)
    set_target_properties(PlatformExemple PROPERTIES
                          MACOSX_BUNDLE TRUE
                          MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Platform/MacOS/Bundle/MacOSXBundleInfo.plist.in"

                          MACOSX_BUNDLE_BUNDLE_NAME "Platform Exemple"
                          MACOSX_BUNDLE_GUI_IDENTIFIER "com.nebula.platformexemple"
                          XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.nebula.platformexemple"
                          MACOSX_BUNDLE_BUNDLE_VERSION "1"
                          MACOSX_BUNDLE_SHORT_VERSION_STRING "dev"
                          MACOSX_BUNDLE_COPYRIGHT "© 2024 - 2026 Infinity"
    )
endif ()
