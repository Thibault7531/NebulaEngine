# Cmake file for Nebula Engine target


# Dependencies
add_subdirectory(vendor)


# Standard Source Files
list(APPEND NebulaSourceFiles
        source/Nebula/Core/Application.cpp
        source/Nebula/Core/Layer.cpp
        source/Nebula/Core/LayerStack.cpp
        source/Nebula/Core/Log.cpp
        source/Nebula/Core/Window.cpp
        source/Nebula/ImGui/ImGuiLayer.cpp
        source/Nebula/Renderer/Buffer.cpp
        source/Nebula/Renderer/Camera.cpp
        source/Nebula/Renderer/CameraController.cpp
        source/Nebula/Renderer/Framebuffer.cpp
        source/Nebula/Renderer/RenderCommand.cpp
        source/Nebula/Renderer/Renderer.cpp
        source/Nebula/Renderer/Renderer2D.cpp
        source/Nebula/Renderer/RendererAPI.cpp
        source/Nebula/Renderer/Shader.cpp
        source/Nebula/Renderer/Texture.cpp
        source/Nebula/Renderer/VertexArray.cpp
)

list(APPEND NebulaHeaderFiles
        source/Nebula.h
        source/Nebula/Core/Application.h
        source/Nebula/Core/EntryPoint.h
        source/Nebula/Core/Input.h
        source/Nebula/Core/KeyCodes.h
        source/Nebula/Core/Core.h
        source/Nebula/Core/Layer.h
        source/Nebula/Core/LayerStack.h
        source/Nebula/Core/Log.h
        source/Nebula/Core/MouseButtonCodes.h
        source/Nebula/Core/Window.h
        source/Nebula/Core/Timestep.h
        source/Nebula/Debug/Instrumentor.h
        source/Nebula/Events/ApplicationEvent.h
        source/Nebula/Events/Event.h
        source/Nebula/Events/KeyEvent.h
        source/Nebula/Events/MouseEvent.h
        source/Nebula/ImGui/ImGuiLayer.h
        source/Nebula/Renderer/Buffer.h
        source/Nebula/Renderer/Camera.h
        source/Nebula/Renderer/CameraController.h
        source/Nebula/Renderer/Framebuffer.h
        source/Nebula/Renderer/GraphicsContext.h
        source/Nebula/Renderer/RenderCommand.h
        source/Nebula/Renderer/Renderer.h
        source/Nebula/Renderer/Renderer2D.h
        source/Nebula/Renderer/RendererAPI.h
        source/Nebula/Renderer/Shader.h
        source/Nebula/Renderer/Texture.h
        source/Nebula/Renderer/VertexArray.h
)


# Windows Source Files
if (nebula_cmake_platform STREQUAL Windows)
    list(APPEND NebulaSourceFiles
            source/Platform/OpenGL/OpenGLBuffer.cpp
            source/Platform/OpenGL/OpenGLContext.cpp
            source/Platform/OpenGL/OpenGLFramebuffer.cpp
            source/Platform/OpenGL/OpenGLRendererAPI.cpp
            source/Platform/OpenGL/OpenGLShader.cpp
            source/Platform/OpenGL/OpenGLTexture.cpp
            source/Platform/OpenGL/OpenGLVertexArray.cpp
            source/Platform/Windows/WindowsWindow.cpp
            source/Platform/Windows/WindowsInput.cpp
            vendor/imgui/backends/imgui_impl_sdl3.cpp
            vendor/imgui/backends/imgui_impl_opengl3.cpp
    )
    list(APPEND NebulaHeaderFiles
            source/Platform/OpenGL/OpenGLBuffer.h
            source/Platform/OpenGL/OpenGLContext.h
            source/Platform/OpenGL/OpenGLFramebuffer.h
            source/Platform/OpenGL/OpenGLRendererAPI.h
            source/Platform/OpenGL/OpenGLShader.h
            source/Platform/OpenGL/OpenGLTexture.h
            source/Platform/OpenGL/OpenGLVertexArray.h
            source/Platform/Windows/WindowsWindow.h
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
target_compile_definitions(Nebula PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)

if (CMAKE_BUILD_TYPE EQUAL "Debug")
    target_compile_definitions(Nebula PRIVATE NB_ENGINE_DEBUG)
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
        vendor
        vendor/Glad/include
        vendor/glm
        vendor/glm/glm
        vendor/imgui
        vendor/SDL3/include
        vendor/SDL3/include/SDL3
        vendor/stb_image
)