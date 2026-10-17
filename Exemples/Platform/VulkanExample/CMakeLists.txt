# CMake file for Vulkan Plarform example target


# Setup Target
if (nebula_cmake_platform STREQUAL MacOS)
    add_executable(PlatformVulkanExample MACOSX_BUNDLE source/PlatformApp.cpp)
else ()
    add_executable(PlatformVulkanExample source/PlatformApp.cpp)
endif ()


# Output Directory
set_target_properties(PlatformVulkanExample PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/PlatformVulkanExample")
target_link_libraries(PlatformVulkanExample PRIVATE Nebula-Platform slang::slang)


target_include_directories(PlatformVulkanExample PRIVATE
                           ../../../NebulaEngine/source
                           ../../../NebulaEngine/vendor/spdlog/include
                           $ENV{WINDOWS_SDK_INCLUDE_DIR}/um
                           ../../../NebulaEngine/vendor/volk
                           ../../../NebulaEngine/vendor/vma-hpp/include
                           ../../../NebulaEngine/vendor/vma-hpp/VulkanMemoryAllocator/include
                           ../../../NebulaEngine/vendor/Vulkan-Headers/include
)
target_include_directories(PlatformVulkanExample PRIVATE $ENV{GENERATED_DIR})


# Compile Options
if (nebula_cmake_platform STREQUAL MacOS)
    add_custom_target(PlatformVulkanExampleCopyPlatformVulkanExampleAssets
                      COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets/" "$<TARGET_BUNDLE_CONTENT_DIR:PlatformVulkanExample>/assets/PlatformVulkanExample"
                      COMMENT "Copying PlatformVulkanExample assets to build directory..."
    )
else ()
    add_custom_target(PlatformVulkanExampleCopyPlatformVulkanExampleAssets
                      COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets/" "$<TARGET_FILE_DIR:PlatformVulkanExample>/assets/PlatformVulkanExample"
                      COMMENT "Copying PlatformVulkanExample assets to build directory..."
    )
endif ()
add_dependencies(PlatformVulkanExample PlatformVulkanExampleCopyPlatformVulkanExampleAssets)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET PlatformVulkanExample PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
    target_compile_definitions(PlatformVulkanExample PRIVATE _DEBUG)
else ()
    set_property(TARGET PlatformVulkanExample PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif ()
target_compile_definitions(PlatformVulkanExample PUBLIC NB_PLATFORM_APP_NAME="PlatformVulkanExample")
target_compile_definitions(PlatformVulkanExample PUBLIC VK_NO_PROTOTYPES)
target_compile_definitions(PlatformVulkanExample PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)


if (nebula_cmake_platform STREQUAL Windows)
    add_custom_command(
            TARGET PlatformVulkanExample
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy "${CMAKE_BINARY_DIR}/generated/deps/Microsoft.WindowsAppSDK.Foundation.2.0.21/runtimes/win-x64/native/Microsoft.WindowsAppRuntime.Bootstrap.dll" "$<TARGET_FILE_DIR:PlatformVulkanExample>/Microsoft.WindowsAppRuntime.Bootstrap.dll"
    )

elseif (nebula_cmake_platform STREQUAL MacOS)
    set_target_properties(PlatformVulkanExample PROPERTIES
                          MACOSX_BUNDLE TRUE
                          MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/NebulaEngine/source/Platform/Platform/MacOS/Bundle/MacOSXBundleInfo.plist.in"

                          MACOSX_BUNDLE_BUNDLE_NAME "Platform Exemple"
                          MACOSX_BUNDLE_GUI_IDENTIFIER "com.nebula.PlatformVulkanExample"
                          XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.nebula.PlatformVulkanExample"
                          MACOSX_BUNDLE_BUNDLE_VERSION "1"
                          MACOSX_BUNDLE_SHORT_VERSION_STRING "dev"
                          MACOSX_BUNDLE_COPYRIGHT "© 2024 - 2026 Infinity"
    )
endif ()