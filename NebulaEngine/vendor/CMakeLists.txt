# Assimp

set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE INTERNAL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE INTERNAL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
add_subdirectory(Assimp)

target_include_directories(assimp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Assimp/contrib/unzip")
if (CMAKE_BUILD_TYPE EQUAL "Debug")
    set_property(TARGET assimp PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
else ()
    set_property(TARGET assimp PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif ()

# cppwinrt

add_subdirectory(winrt)


# EnTT

add_subdirectory(entt)


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


# vma-hpp

add_subdirectory(vma-hpp)


# volk

add_subdirectory(volk)
if (nebula_cmake_platform STREQUAL Windows)
    target_compile_definitions(volk PRIVATE VK_USE_PLATFORM_WIN32_KHR)
endif ()


# Vulkan




# YAML

option(YAML_MSVC_SHARED_RT "" OFF)
add_subdirectory(yaml-cpp)