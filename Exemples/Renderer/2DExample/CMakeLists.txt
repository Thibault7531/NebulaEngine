# CMake file for 2D renderer example target


# Setup Target
if (nebula_cmake_platform STREQUAL MacOS)
    add_executable(Renderer2DExample MACOSX_BUNDLE
            source/ExampleApp.cpp
            source/RendererLayer.cpp
    )
else ()
    add_executable(Renderer2DExample
            source/ExampleApp.cpp
            source/RendererLayer.cpp
    )
endif ()


# Output Directory
set_target_properties(Renderer2DExample PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/Renderer2DExample")
target_link_libraries(Renderer2DExample PRIVATE Nebula)


target_include_directories(Renderer2DExample PRIVATE
                           ../../../NebulaEngine/source
                           ../../../NebulaEngine/source/Core
                           ../../../NebulaEngine/source/Platform
                           ../../../NebulaEngine/vendor/Assimp/include
                           ${CMAKE_BINARY_DIR}/NebulaEngine/vendor/Assimp/include
                           ../../../NebulaEngine/vendor/spdlog/include
                           ../../../NebulaEngine/vendor/SDL3/include/SDL3
                           ../../../NebulaEngine/vendor/SDL3/include
                           ../../../NebulaEngine/vendor/stb_image
                           ../../../NebulaEngine/vendor/imgui
                           ../../../NebulaEngine/vendor/entt/single_include
                           ../../../NebulaEngine/vendor/Glad/include
                           ../../../NebulaEngine/vendor/glm
                           ../../../NebulaEngine/vendor/glm/glm
                           ../../../NebulaEngine/vendor/metal-cpp
                           ../../../NebulaEngine/vendor/vma-hpp/include
                           ../../../vendor/vma-hpp/VulkanMemoryAllocator/include
                           ../../../NebulaEngine/vendor/Vulkan-Headers/include
)


# Compile Options
if (nebula_cmake_platform STREQUAL MacOS)
    add_custom_target(Renderer2DExampleCopyRenderer2DExampleAssets
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets/" "$<TARGET_BUNDLE_CONTENT_DIR:Renderer2DExample>/assets/Renderer2DExample"
            COMMENT "Copying Renderer2DExample assets to build directory..."
    )
    add_custom_target(Renderer2DExampleCopyNebulaEngineAssets
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/NebulaEngine/assets/" "$<TARGET_BUNDLE_CONTENT_DIR:Renderer2DExample>/assets/NebulaEngine"
            COMMENT "Copying NebulaEngine assets to build directory..."
    )
else ()
    add_custom_target(Renderer2DExampleCopyRenderer2DExampleAssets
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets/" "$<TARGET_FILE_DIR:Renderer2DExample>/assets/Renderer2DExample"
            COMMENT "Copying Renderer2DExample assets to build directory..."
    )
    add_custom_target(Renderer2DExampleCopyNebulaEngineAssets
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/NebulaEngine/assets/" "$<TARGET_FILE_DIR:Renderer2DExample>/assets/NebulaEngine"
            COMMENT "Copying NebulaEngine assets to build directory..."
    )
endif ()
add_dependencies(Renderer2DExample Renderer2DExampleCopyRenderer2DExampleAssets Renderer2DExampleCopyNebulaEngineAssets)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET Renderer2DExample PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
    target_compile_definitions(Renderer2DExample PRIVATE _DEBUG)
else ()
    set_property(TARGET Renderer2DExample PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif ()
target_compile_definitions(Renderer2DExample PUBLIC NB_PLATFORM_APP_NAME="Renderer2DExample")


if (nebula_cmake_platform STREQUAL Windows)
    add_custom_command(
            TARGET Renderer2DExample
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy "${CMAKE_BINARY_DIR}/generated/deps/Microsoft.WindowsAppSDK.Foundation.2.0.21/runtimes/win-x64/native/Microsoft.WindowsAppRuntime.Bootstrap.dll" "$<TARGET_FILE_DIR:Renderer2DExample>/Microsoft.WindowsAppRuntime.Bootstrap.dll"
    )
    add_custom_command(
            TARGET Renderer2DExample
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy "${CMAKE_BINARY_DIR}/_deps/slang_prebuilt-src/bin/slang.dll" "$<TARGET_FILE_DIR:Renderer2DExample>/slang.dll"
    )
    add_custom_command(
            TARGET Renderer2DExample
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy "${CMAKE_BINARY_DIR}/_deps/slang_prebuilt-src/bin/slang-compiler.dll" "$<TARGET_FILE_DIR:Renderer2DExample>/slang-compiler.dll"
    )
    add_custom_command(
            TARGET Renderer2DExample
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy "${CMAKE_BINARY_DIR}/_deps/slang_prebuilt-src/bin/slang-glslang.dll" "$<TARGET_FILE_DIR:Renderer2DExample>/slang-glslang.dll"
    )

elseif (nebula_cmake_platform STREQUAL MacOS)
    set_target_properties(Renderer2DExample PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Platform/MacOS/Bundle/MacOSXBundleInfo.plist.in"

            MACOSX_BUNDLE_BUNDLE_NAME "Platform Exemple"
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.nebula.Renderer2DExample"
            XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.nebula.Renderer2DExample"
            MACOSX_BUNDLE_BUNDLE_VERSION "1"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "dev"
            MACOSX_BUNDLE_COPYRIGHT "© 2024 - 2026 Infinity"
    )
endif ()
