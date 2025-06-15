# Cmake file for Nebula Engine target


# Dependencies
add_subdirectory(vendor)


# Standard Source Files
file(GLOB_RECURSE NebulaSourceFiles
        "source/Nebula/*.cpp"
)

file(GLOB_RECURSE NebulaHeaderFiles
        "source/Nebula/*.h"
        "source/Nebula/*.hpp"
)


# Windows Source Files
if (nebula_cmake_platform STREQUAL Windows)
    file(GLOB_RECURSE NebulaPlatformSourceFiles
            "source/Platform/Windows/*.cpp"
            "source/Platform/OpenGL/*.cpp"
    )
    file(GLOB_RECURSE NebulaPlatformHeaderFiles
            "source/Platform/Windows/*.hpp"
            "source/Platform/Windows/*.h"
            "source/Platform/OpenGL/*.hpp"
            "source/Platform/OpenGL/*.h"
    )
    list(APPEND NebulaSourceFiles ${NebulaPlatformSourceFiles})
    list(APPEND NebulaHeaderFiles ${NebulaPlatformHeaderFiles})
endif ()


# Setup Target
add_library(Nebula STATIC)
target_sources(Nebula PRIVATE ${NebulaSourceFiles} PUBLIC ${NebulaHeaderFiles})


# Output Directory
set_target_properties(Nebula PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/NebulaEngine")


# Compilation Options
set_target_properties(Nebula PROPERTIES COMPILE_OPTIONS "/utf-8")
add_custom_command(
        TARGET Nebula
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy $<TARGET_FILE:SDL3-shared> $<TARGET_FILE_DIR:TestGame>/$<TARGET_FILE_NAME:SDL3-shared>
)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET Nebula PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
else ()
    set_property(TARGET Nebula PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif ()


# Defines
target_compile_definitions(Nebula PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Nebula PRIVATE NB_ENGINE_DEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(Nebula PRIVATE NB_RELEASE)
else ()
    target_compile_definitions(Nebula PRIVATE NB_DIST)
endif ()


#Libraries
target_precompile_headers(Nebula PRIVATE source/nbpch.h)
target_link_libraries(Nebula PRIVATE
        ImGui
        SDL3-shared
        GLAD
        spdlog::spdlog_header_only
        EnTT::EnTT
)
target_include_directories(Nebula PRIVATE
        source
        vendor
        vendor/entt/single_include
        vendor/Glad/include
        vendor/glm
        vendor/glm/glm
        vendor/imgui
        vendor/SDL3/include
        vendor/SDL3/include/SDL3
        vendor/stb_image
)