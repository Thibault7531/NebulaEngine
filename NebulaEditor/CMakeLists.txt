# Cmake file for Nebula Editor target


# Source files
file(GLOB_RECURSE EditorSourceList
        "source/*.cpp"
)


# Header files
file(GLOB_RECURSE EditorHeaderList
        "source/*.h"
)


# Setup Target
add_executable(NebulaEditor)
target_sources(NebulaEditor PRIVATE ${EditorSourceList} PUBLIC ${EditorHeaderList})


# Output Directory
set_target_properties(NebulaEditor PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/NebulaEditor")


# Compile Options
set_target_properties(NebulaEditor PROPERTIES COMPILE_OPTIONS "/utf-8")
add_custom_command(
        TARGET NebulaEditor
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets/" "$<TARGET_FILE_DIR:NebulaEditor>/assets/NebulaEditor"
)
add_custom_command(
        TARGET NebulaEditor
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy_directory "${CMAKE_SOURCE_DIR}/NebulaEngine/assets/" "$<TARGET_FILE_DIR:NebulaEditor>/assets/NebulaEngine"
)
add_custom_command(
        TARGET NebulaEditor
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy $<TARGET_FILE:SDL3-shared> $<TARGET_FILE_DIR:NebulaEditor>/$<TARGET_FILE_NAME:SDL3-shared>
)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET NebulaEditor PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
else ()
    set_property(TARGET NebulaEditor PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif ()


# Libraries
target_link_libraries(NebulaEditor PUBLIC Nebula)
target_include_directories(NebulaEditor PRIVATE
        ../NebulaEngine/source
        ../NebulaEngine/vendor/spdlog/include
        ../NebulaEngine/vendor/SDL3/include/SDL3
        ../NebulaEngine/vendor/SDL3/include
        ../NebulaEngine/vendor/imgui
        ../NebulaEngine/vendor/entt/single_include
        ../NebulaEngine/vendor/Glad/include
        ../NebulaEngine/vendor/glm
        ../NebulaEngine/vendor/glm/glm
)
