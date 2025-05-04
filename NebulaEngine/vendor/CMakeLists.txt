# EnTT

add_subdirectory(entt)


# GLAD

add_library(GLAD STATIC Glad/src/glad.c Glad/include/glad/glad.h Glad/include/KHR/khrplatform.h)

target_include_directories(GLAD PUBLIC Glad/include)

if (MSVC)
    if (CMAKE_BUILD_TYPE EQUAL "Debug")
        set_property(TARGET GLAD PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
    else ()
        set_property(TARGET GLAD PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
    endif ()
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


# SDL

add_subdirectory(SDL3)


# spdlog

add_subdirectory(spdlog)