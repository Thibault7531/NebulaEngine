# Cmake file for Nebula Engine - Platform target


# Standard Source Files
file(GLOB_RECURSE NebulaPlatformCoreSourceFiles
     "Nebula/*.cpp"
)

file(GLOB_RECURSE NebulaPlatformCoreHeaderFile
     "Nebula/*.h"
     "Nebula/*.hpp"
)


# Windows Source Files
if (nebula_cmake_platform STREQUAL Windows)
    file(GLOB_RECURSE NebulaPlatformOSSourceFiles
         "Platform/Windows/*.cpp"
    )
    file(GLOB_RECURSE NebulaPlatformOSHeaderFiles
         "Platform/Windows/*.h"
         "Platform/Windows/*.hpp"
    )
    list(APPEND NebulaPlatformCoreSourceFiles ${NebulaPlatformOSSourceFiles})
    list(APPEND NebulaPlatformCoreHeaderFiles ${NebulaPlatformOSHeaderFiles})
endif ()


# Setup Target
add_library(Nebula-Platform STATIC)
target_sources(Nebula-Platform PRIVATE ${NebulaPlatformCoreSourceFiles} PUBLIC ${NebulaPlatformCoreHeaderFiles})


# Output Directory
set_target_properties(Nebula-Platform PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/NebulaEngine")


# Compilation Options
set_target_properties(Nebula-Platform PROPERTIES COMPILE_OPTIONS "/utf-8")
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
        target_compile_options(Nebula-Platform PRIVATE /Od)
        target_compile_definitions(Nebula-Platform PRIVATE _DEBUG WINRT_NO_MAKE_DETECTION)
    endif()
endif ()