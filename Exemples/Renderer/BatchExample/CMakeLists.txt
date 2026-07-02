# CMake file for 2D renderer example target


# Setup Target
if (nebula_cmake_platform STREQUAL MacOS)
    add_executable(BatchRendererExample MACOSX_BUNDLE
            source/ExampleApp.cpp
            source/RendererLayer.cpp
    )
else ()
    add_executable(BatchRendererExample
            source/ExampleApp.cpp
            source/RendererLayer.cpp
    )
endif ()


# Output Directory
set_target_properties(BatchRendererExample PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/BatchRendererExample")
target_link_libraries(BatchRendererExample PRIVATE Nebula)


target_include_directories(BatchRendererExample PRIVATE
        ../../../NebulaEngine/source
        ../../../NebulaEngine/source/Core
        ../../../NebulaEngine/source/Platform
        ../../../NebulaEngine/vendor/Assimp/include
        ${CMAKE_BINARY_DIR}/NebulaEngine/vendor/Assimp/include
        ../../../NebulaEngine/vendor/spdlog/include
        ../../../NebulaEngine/vendor/SDL3/include/SDL3
        ../../../NebulaEngine/vendor/SDL3/include
        ../../../NebulaEngine/vendor/imgui
        ../../../NebulaEngine/vendor/entt/single_include
        ../../../NebulaEngine/vendor/Glad/include
        ../../../NebulaEngine/vendor/glm
        ../../../NebulaEngine/vendor/glm/glm
)


# Compile Options
if (nebula_cmake_platform STREQUAL MacOS)
    add_custom_target(BatchRendererExampleCopyBatchRendererExampleAssets
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets/" "$<TARGET_BUNDLE_CONTENT_DIR:BatchRendererExample>/assets/BatchRendererExample"
            COMMENT "Copying BatchRendererExample assets to build directory..."
    )
    add_custom_target(BatchRendererExampleCopyNebulaEngineAssets
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/NebulaEngine/assets/" "$<TARGET_BUNDLE_CONTENT_DIR:BatchRendererExample>/assets/NebulaEngine"
            COMMENT "Copying NebulaEngine assets to build directory..."
    )
else ()
    add_custom_target(BatchRendererExampleCopyBatchRendererExampleAssets
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets/" "$<TARGET_FILE_DIR:BatchRendererExample>/assets/BatchRendererExample"
            COMMENT "Copying BatchRendererExample assets to build directory..."
    )
    add_custom_target(BatchRendererExampleCopyNebulaEngineAssets
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/NebulaEngine/assets/" "$<TARGET_FILE_DIR:BatchRendererExample>/assets/NebulaEngine"
            COMMENT "Copying NebulaEngine assets to build directory..."
    )
endif ()
add_dependencies(BatchRendererExample BatchRendererExampleCopyBatchRendererExampleAssets BatchRendererExampleCopyNebulaEngineAssets)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET BatchRendererExample PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
    target_compile_definitions(BatchRendererExample PRIVATE _DEBUG)
else ()
    set_property(TARGET BatchRendererExample PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif ()
target_compile_definitions(BatchRendererExample PUBLIC NB_PLATFORM_APP_NAME="BatchRendererExample")


if (nebula_cmake_platform STREQUAL Windows)
    add_custom_command(
            TARGET BatchRendererExample
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy "${CMAKE_BINARY_DIR}/generated/deps/Microsoft.WindowsAppSDK.Foundation.2.0.21/runtimes/win-x64/native/Microsoft.WindowsAppRuntime.Bootstrap.dll" "$<TARGET_FILE_DIR:BatchRendererExample>/Microsoft.WindowsAppRuntime.Bootstrap.dll"
    )

elseif (nebula_cmake_platform STREQUAL MacOS)
    set_target_properties(BatchRendererExample PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Platform/MacOS/Bundle/MacOSXBundleInfo.plist.in"

            MACOSX_BUNDLE_BUNDLE_NAME "Platform Exemple"
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.nebula.BatchRendererExample"
            XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.nebula.BatchRendererExample"
            MACOSX_BUNDLE_BUNDLE_VERSION "1"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "dev"
            MACOSX_BUNDLE_COPYRIGHT "© 2024 - 2026 Infinity"
    )
endif ()
