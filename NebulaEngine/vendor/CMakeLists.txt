# Assimp

set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE INTERNAL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE INTERNAL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
add_subdirectory(Assimp)

if (CMAKE_BUILD_TYPE EQUAL "Debug")
    set_property(TARGET assimp PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
else ()
    set_property(TARGET assimp PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif ()

# cppwinrt

add_subdirectory(winrt)


# EnTT

add_subdirectory(entt)


# GLAD

add_library(GLAD STATIC Glad/src/gl.c Glad/include/glad/gl.h Glad/include/KHR/khrplatform.h)
add_library(GLAD_WGL STATIC Glad/src/wgl.c Glad/include/glad/wgl.h Glad/include/KHR/khrplatform.h)
add_library(GLAD_EGL STATIC Glad/src/egl.c Glad/include/EGL/eglplatform.h Glad/include/glad/egl.h Glad/include/KHR/khrplatform.h)

target_include_directories(GLAD PUBLIC Glad/include)
target_include_directories(GLAD_WGL PUBLIC Glad/include)
target_include_directories(GLAD_EGL PUBLIC Glad/include)

if (CMAKE_BUILD_TYPE EQUAL "Debug")
    set_property(TARGET GLAD PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
    set_property(TARGET GLAD_WGL PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
else ()
    set_property(TARGET GLAD PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
    set_property(TARGET GLAD_WGL PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif ()


# glm

add_subdirectory(glm)


# ImGui

add_library(ImGui STATIC
        imgui/imconfig.h
        imgui/imgui.h
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_internal.h
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imstb_rectpack.h
        imgui/imstb_textedit.h
        imgui/imstb_truetype.h
        imgui/imgui_demo.cpp
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET ImGui PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
else ()
    set_property(TARGET ImGui PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif ()


# ImGuizmo

add_library(ImGuizmo STATIC
            ImGuizmo/src/GraphEditor.h
            ImGuizmo/src/GraphEditor.cpp
            ImGuizmo/src/ImCurveEdit.h
            ImGuizmo/src/ImCurveEdit.cpp
            ImGuizmo/src/ImGradient.h
            ImGuizmo/src/ImGradient.cpp
            ImGuizmo/src/ImGuizmo.h
            ImGuizmo/src/ImGuizmo.cpp
            ImGuizmo/src/ImLightRig.h
            ImGuizmo/src/ImSequencer.h
            ImGuizmo/src/ImSequencer.cpp
            ImGuizmo/src/ImZoomSlider.h
)

target_include_directories(ImGuizmo PRIVATE
                           imgui
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET ImGuizmo PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
else ()
    set_property(TARGET ImGuizmo PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif ()


# SDL3

add_subdirectory(SDL)


# Slang

add_subdirectory(slang)


# spdlog

add_subdirectory(spdlog)


# YAML

option(YAML_MSVC_SHARED_RT "" OFF)
add_subdirectory(yaml-cpp)