#include <glad/gl.h>

#include "Platform/Nebula/CoreSystems/Core/CoreUtils.h"
#include "Platform/Nebula/CoreSystems/Core/Log.h"
#include "Platform/Nebula/CoreSystems/Events/Event.h"
#include "Platform/Nebula/CoreSystems/Events/KeyEvent.h"
#include "Platform/Nebula/CoreSystems/Events/MouseEvent.h"
#include "Platform/Nebula/CoreSystems/Events/WindowEvents.h"
#include "Platform/Nebula/CoreSystems/Windows/WindowManager.h"
#include "Platform/Nebula/Dialogs/FileDialogs.h"

std::string vertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 vColor;
void main()
{
    vColor = aColor;
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)";

std::string fragmentShader = R"(
#version 330 core
layout(location = 0) out vec4 color;
in vec3 vColor;
void main()
{
    color = vec4(vColor.x, vColor.y, vColor.z, 1.0);
}
)";

static bool s_running = true;

bool OnKeyPressed(Nebula::Platform::KeyPressedEvent& event)
{
    if (event.GetKeyCode() == KeyboardKey::O)
    {
        Nebula::Platform::FileDialogs::OpenFile(Nebula::Platform::FileType::NebulaScene, [](std::string path) { NB_PLATFORM_INFO("File opened: {}", path); });
    }
    if (event.GetKeyCode() == KeyboardKey::S)
    {
        Nebula::Platform::FileDialogs::SaveFile(Nebula::Platform::FileType::NebulaScene, [](std::string path) { NB_PLATFORM_INFO("File saved: {}", path); });
    }

    return true;
}

void Callback(Nebula::Platform::Event& event)
{
    if (event.GetEventType() == Nebula::Platform::EventType::WindowClosed) s_running = false;

    Nebula::Platform::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Nebula::Platform::KeyPressedEvent>(OnKeyPressed);
}

int main()
{
    Nebula::Platform::Log::Init();

    Nebula::Platform::CoreUtils::InitThread();
    Nebula::Ref<Nebula::Platform::Window> window = Nebula::Platform::WindowManager::Get().CreateMainWindow({.Style = Nebula::Platform::WindowStyle::Default, .API = Nebula::Platform::GraphicsAPI::OpenGL410});
    window->SetEventCallback(Callback);

    const char* vertexCode = vertexShader.c_str();
    int vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vertexCode, nullptr);
    glCompileShader(vShader);

    const char* fragmentCode = fragmentShader.c_str();
    int pShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(pShader, 1, &fragmentCode, nullptr);
    glCompileShader(pShader);

    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vShader);
    glAttachShader(shaderProgram, pShader);
    glLinkProgram(shaderProgram);

    uint32_t vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    uint32_t vbo;
    //glCreateBuffers(1, &vbo);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    while (s_running)
    {
        window->GetSurface()->NewFrame();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(vao);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        window->Present(0);
        window->OnUpdate();
    }

    window.reset();

    return 0;
}
