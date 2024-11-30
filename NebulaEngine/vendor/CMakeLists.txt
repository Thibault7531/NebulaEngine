# GLAD

add_library(GLAD STATIC Glad/src/glad.c Glad/include/glad/glad.h Glad/include/KHR/khrplatform.h)

target_include_directories(GLAD PUBLIC Glad/include)


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
target_compile_definitions(ImGui PRIVATE "IMGUI_API=__declspec(dllexport)")


# SDL

add_subdirectory(SDL3)


# spdlog

add_subdirectory(spdlog)