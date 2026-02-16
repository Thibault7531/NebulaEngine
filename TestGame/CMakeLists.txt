# CMake file for TestGame target


# Setup Target
add_executable(TestGame source/TestGame.cpp source/Sandbox2D.h source/Sandbox2D.cpp)


# Output Directory
set_target_properties(TestGame PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/TestGame")


# Compile Options
add_custom_command(
        TARGET TestGame
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets/" "$<TARGET_FILE_DIR:TestGame>/assets/TestGame"
)
add_custom_command(
        TARGET TestGame
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy_directory "${CMAKE_SOURCE_DIR}/NebulaEngine/assets/" "$<TARGET_FILE_DIR:TestGame>/assets/NebulaEngine"
)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET TestGame PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
else ()
    set_property(TARGET TestGame PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif ()


# Libraries
target_link_libraries(TestGame PUBLIC Nebula)
target_include_directories(TestGame PRIVATE
        ../NebulaEngine/source
        ../NebulaEngine/source/Core
        ../NebulaEngine/source/Platform
        ../NebulaEngine/vendor/Assimp/include
        ${CMAKE_BINARY_DIR}/NebulaEngine/vendor/Assimp/include
        ../NebulaEngine/vendor/spdlog/include
        ../NebulaEngine/vendor/SDL3/include/SDL3
        ../NebulaEngine/vendor/SDL3/include
        ../NebulaEngine/vendor/imgui
        ../NebulaEngine/vendor/entt/single_include
        ../NebulaEngine/vendor/Glad/include
        ../NebulaEngine/vendor/glm
        ../NebulaEngine/vendor/glm/glm
)

if (nebula_cmake_platform STREQUAL Windows)
    set_target_properties(TestGame PROPERTIES COMPILE_OPTIONS "/utf-8")
    add_custom_command(
            TARGET TestGame
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy "${CMAKE_SOURCE_DIR}/NebulaEngine/vendor/winrt/Microsoft.WindowsAppRuntime.Bootstrap.dll" "$<TARGET_FILE_DIR:TestGame>/Microsoft.WindowsAppRuntime.Bootstrap.dll"
    )
endif ()