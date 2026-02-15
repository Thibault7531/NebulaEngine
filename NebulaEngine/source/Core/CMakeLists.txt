# Cmake file for Nebula Engine - Core target

# Standard Source Files
file(GLOB_RECURSE NebulaSourceFiles
     "Nebula/*.cpp"
)

file(GLOB_RECURSE NebulaHeaderFiles
     "Nebula/*.h"
     "Nebula/*.hpp"
)


# Windows Source Files
if (nebula_cmake_platform STREQUAL Windows)
    file(GLOB_RECURSE NebulaPlatformSourceFiles
         "Platform/Windows/*.cpp"
         "Platform/OpenGL/*.cpp"
    )
    file(GLOB_RECURSE NebulaPlatformHeaderFiles
         "Platform/Windows/*.hpp"
         "Platform/Windows/*.h"
         "Platform/OpenGL/*.hpp"
         "Platform/OpenGL/*.h"
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
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET Nebula PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
else ()
    set_property(TARGET Nebula PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif ()


# Defines
target_compile_definitions(Nebula PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Nebula PUBLIC NB_ENGINE_DEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(Nebula PUBLIC NB_RELEASE)
else ()
    target_compile_definitions(Nebula PUBLIC NB_DIST)
endif ()


#Libraries
target_precompile_headers(Nebula PUBLIC ../nbpch.h)
target_link_libraries(Nebula PRIVATE
                      Nebula-Platform
                      assimp::assimp
                      EnTT::EnTT
                      GLAD
                      ImGui
                      spdlog::spdlog_header_only
                      yaml-cpp::yaml-cpp
)
target_include_directories(Nebula PRIVATE
                           ../
                           ./
                           ../../vendor
                           ../../vendor/Assimp/include
                           ${CMAKE_BINARY_DIR}/NebulaEngine/vendor/Assimp/include
                           ../../vendor/entt/single_include
                           ../../vendor/Glad/include
                           ../../vendor/glm
                           ../../vendor/glm/glm
                           ../../vendor/imgui
                           ../../vendor/stb_image
                           ../../vendor/yaml-cpp/include
)


# Windows Settings
if (nebula_cmake_platform STREQUAL Windows)
    target_compile_definitions(Nebula PUBLIC NB_PLATFORM_WINDOWS)
    set(WIN32 True)

    target_link_libraries(Nebula PRIVATE
                          dwmapi
                          opengl32
    )
endif ()