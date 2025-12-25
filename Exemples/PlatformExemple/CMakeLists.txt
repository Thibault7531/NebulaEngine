# CMake file for PlatformExemple target


# Setup Target
add_executable(PlatformExemple source/PlatformApp.cpp)


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
set_target_properties(PlatformExemple PROPERTIES COMPILE_OPTIONS "/utf-8")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET PlatformExemple PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
    target_compile_options(PlatformExemple PRIVATE -0d -Zi)
    target_link_options(PlatformExemple PRIVATE /DEBUG:FULL)
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
endif ()
