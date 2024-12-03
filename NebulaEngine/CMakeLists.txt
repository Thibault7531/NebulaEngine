# Cmake file for Nebula Engine target


# Dependencies
add_subdirectory(vendor)


# Standard Source Files
list(APPEND NebulaSourceFiles
        source/Nebula/Application.cpp
        source/Nebula/Layer.cpp
        source/Nebula/LayerStack.cpp
        source/Nebula/Log.cpp
        source/Nebula/ImGui/ImGuiLayer.cpp
        source/Nebula/Renderer/Shader.cpp
)

list(APPEND NebulaHeaderFiles
        source/Nebula.h
        source/Nebula/Application.h
        source/Nebula/EntryPoint.h
        source/Nebula/Input.h
        source/Nebula/KeyCodes.h
        source/Nebula/Core.h
        source/Nebula/Layer.h
        source/Nebula/LayerStack.h
        source/Nebula/Log.h
        source/Nebula/MouseButtonCodes.h
        source/Nebula/Window.h
        source/Nebula/Events/ApplicationEvent.h
        source/Nebula/Events/Event.h
        source/Nebula/Events/KeyEvent.h
        source/Nebula/Events/MouseEvent.h
        source/Nebula/ImGui/ImGuiLayer.h
        source/Nebula/Renderer/GraphicsContext.h
        source/Nebula/Renderer/Shader.h
)


# Windows Source Files
if (nebula_cmake_platform STREQUAL Windows)
    list(APPEND NebulaSourceFiles
            source/Platform/OpenGL/OpenGLContext.cpp
            source/Platform/Windows/WindowsWindow.cpp
            source/Platform/Windows/WindowsInput.cpp
            vendor/imgui/backends/imgui_impl_sdl3.cpp
            vendor/imgui/backends/imgui_impl_opengl3.cpp
    )
    list(APPEND NebulaHeaderFiles
            source/Platform/OpenGL/OpenGLContext.h
            source/Platform/Windows/WindowsWindow.h
            source/Platform/Windows/WindowsInput.h
            vendor/imgui/backends/imgui_impl_sdl3.h
            vendor/imgui/backends/imgui_impl_opengl3.h
    )
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
target_compile_definitions(Nebula PRIVATE NB_PLATFORM_WINDOWS)
target_compile_definitions(Nebula PRIVATE NB_BUILD_DLL)
target_compile_definitions(Nebula PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)

if (CMAKE_BUILD_TYPE EQUAL "Debug")
    target_compile_definitions(Nebula PRIVATE NB_DEBUG)
elseif (CMAKE_BUILD_TYPE EQUAL "Release")
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
)
target_include_directories(Nebula PRIVATE
        source
        vendor/Glad/include
        vendor/imgui
        vendor/SDL3/include
        vendor/SDL3/include/SDL3
        vendor/glm
)