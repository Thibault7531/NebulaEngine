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
if (nebula_cmake_platform STREQUAL MacOS)
    add_executable(NebulaEditor MACOSX_BUNDLE)
else ()
    add_executable(NebulaEditor)
endif ()
target_sources(NebulaEditor PRIVATE ${EditorSourceList} PUBLIC ${EditorHeaderList})


# Output Directory
set_target_properties(NebulaEditor PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/NebulaEditor")


# Compile Options
if (nebula_cmake_platform STREQUAL MacOS)
    add_custom_command(
            TARGET NebulaEditor
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets/" "$<TARGET_BUNDLE_CONTENT_DIR:NebulaEditor>/assets/NebulaEditor"
    )
    add_custom_command(
            TARGET NebulaEditor
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy_directory "${CMAKE_SOURCE_DIR}/NebulaEngine/assets/" "$<TARGET_BUNDLE_CONTENT_DIR:NebulaEditor>/assets/NebulaEngine"
    )
else ()
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
endif ()
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET NebulaEditor PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
else ()
    set_property(TARGET NebulaEditor PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif ()
target_compile_definitions(NebulaEditor PUBLIC NB_PLATFORM_APP_NAME="NebulaEditor")


# Libraries
target_link_libraries(NebulaEditor PUBLIC Nebula)
target_include_directories(NebulaEditor PRIVATE
        ../NebulaEngine/source
        ../NebulaEngine/source/Core
        ../NebulaEngine/vendor/Assimp/include
        ${CMAKE_BINARY_DIR}/NebulaEngine/vendor/Assimp/include
        ../NebulaEngine/vendor/spdlog/include
        ../NebulaEngine/vendor/SDL3/include/SDL3
        ../NebulaEngine/vendor/SDL3/include
        ../NebulaEngine/vendor/imgui
        ../NebulaEngine/vendor/ImGuizmo
        ../NebulaEngine/vendor/nanosvg
        ../NebulaEngine/vendor/entt/single_include
        ../NebulaEngine/vendor/Glad/include
        ../NebulaEngine/vendor/glm
        ../NebulaEngine/vendor/glm/glm
        ../NebulaEngine/vendor/yaml-cpp/include
)

if (nebula_cmake_platform STREQUAL Windows)
    target_compile_definitions(NebulaEditor PUBLIC NB_PLATFORM_WINDOWS)
    add_custom_command(
            TARGET NebulaEditor
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy "${CMAKE_SOURCE_DIR}/NebulaEngine/vendor/winrt/Microsoft.WindowsAppRuntime.Bootstrap.dll" "$<TARGET_FILE_DIR:NebulaEditor>/Microsoft.WindowsAppRuntime.Bootstrap.dll"
    )

elseif (nebula_cmake_platform STREQUAL Linux)
    target_compile_definitions(NebulaEditor PUBLIC NB_PLATFORM_LINUX)

elseif (nebula_cmake_platform STREQUAL MacOS)
    target_compile_definitions(NebulaEditor PUBLIC NB_PLATFORM_MACOS)
    set_target_properties(NebulaEditor PROPERTIES
                          MACOSX_BUNDLE TRUE
                          MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Platform/MacOS/Bundle/MacOSXBundleInfo.plist.in"

                          MACOSX_BUNDLE_BUNDLE_NAME "Nebula Editor"
                          MACOSX_BUNDLE_GUI_IDENTIFIER "com.nebula.editor"
                          XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.nebula.editor"
                          MACOSX_BUNDLE_BUNDLE_VERSION "1"
                          MACOSX_BUNDLE_SHORT_VERSION_STRING "dev"
                          MACOSX_BUNDLE_COPYRIGHT "© 2024 - 2026 Infinity"
    )
endif ()