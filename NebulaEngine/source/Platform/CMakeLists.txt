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
elseif (nebula_cmake_platform STREQUAL Linux)
    file(GLOB_RECURSE NebulaPlatformOSSourceFiles
         "Platform/Linux/*.cpp"
    )
    file(GLOB_RECURSE NebulaPlatformOSHeaderFiles
         "Platform/Linux/*.h"
         "Platform/Linux/*.hpp"
    )
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
)


# Windows Settings
if (nebula_cmake_platform STREQUAL Windows)
    target_compile_definitions(Nebula-Platform PUBLIC NB_PLATFORM_WINDOWS)
    set_target_properties(Nebula-Platform PROPERTIES COMPILE_OPTIONS "/utf-8")

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

elseif (nebula_cmake_platform STREQUAL Linux)
    target_compile_definitions(Nebula-Platform PUBLIC NB_PLATFORM_LINUX)

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(WAYLAND REQUIRED wayland-client wayland-egl)
    pkg_check_modules(WAYLAND-PROTOCOLS REQUIRED wayland-protocols)
    pkg_check_modules(EGL REQUIRED egl)

    pkg_get_variable(WAYLAND_PROTOCOLS_DATADIR wayland-protocols pkgdatadir)
    find_program(WAYLAND_SCANNER wayland-scanner)

    # xdg-shell
    set(XDG_SHELL_XML ${WAYLAND_PROTOCOLS_DATADIR}/stable/xdg-shell/xdg-shell.xml)
    set(XDG_SHELL_CODE ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-protocol.c)
    set(XDG_SHELL_HEADER ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-client-protocol.h)
    add_custom_command(
            OUTPUT ${XDG_SHELL_CODE}
            COMMAND ${WAYLAND_SCANNER} private-code ${XDG_SHELL_XML} ${XDG_SHELL_CODE}
            DEPENDS ${XDG_SHELL_XML}
            VERBATIM
    )
    add_custom_command(
            OUTPUT ${XDG_SHELL_HEADER}
            COMMAND ${WAYLAND_SCANNER} client-header ${XDG_SHELL_XML} ${XDG_SHELL_HEADER}
            DEPENDS ${XDG_SHELL_XML}
            VERBATIM
    )
    add_library(XDG_SHELL STATIC ${XDG_SHELL_CODE} ${XDG_SHELL_HEADER})
    target_include_directories(XDG_SHELL PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

    target_link_libraries(Nebula-Platform PRIVATE
                          GLAD_EGL
                          ${WAYLAND_LIBRARIES}
                          ${EGL_LIBRARIES}
                          XDG_SHELL
    )

    target_include_directories(Nebula-Platform PUBLIC
                               ${WAYLAND_INCLUDE_DIRS}
                               ${EGL_INCLUDE_DIRS}
    )
endif ()