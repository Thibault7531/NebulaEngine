# CMake file for TestGame target


# Setup Target
add_executable(TestGame source/TestGame.cpp)


# Output Directory
set_target_properties(TestGame PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/TestGame")


# Compile Options
set_target_properties(TestGame PROPERTIES COMPILE_OPTIONS "/utf-8")
add_custom_command(
        TARGET TestGame
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets/" "$<TARGET_FILE_DIR:TestGame>/assets"
)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET TestGame PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
else ()
    set_property(TARGET TestGame PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif ()


# Defines
target_compile_definitions(TestGame PUBLIC NB_PLATFORM_WINDOWS)


# Libraries
target_link_libraries(TestGame PUBLIC Nebula)
target_include_directories(TestGame PRIVATE
        ../NebulaEngine/source
        ../NebulaEngine/vendor/spdlog/include
        ../NebulaEngine/vendor/SDL3/include/SDL3
        ../NebulaEngine/vendor/SDL3/include
        ../NebulaEngine/vendor/imgui
        ../NebulaEngine/vendor/Glad/include
        ../NebulaEngine/vendor/glm
)
