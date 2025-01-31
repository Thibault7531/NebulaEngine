#include "Sandbox2D.h"

#include <Nebula/Debug/Instrumentor.h>

#include "glm/gtc/type_ptr.hpp"
#include "Platform/OpenGL/OpenGLShader.h"

Sandbox2D::Sandbox2D()
    : Layer("Sandbox2D"), m_CameraController(16.0f/9.0f)
{
}

void Sandbox2D::OnAttach()
{
    m_Craft = Nebula::Texture2D::Create("assets/textures/crafting_table_front.png");
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Nebula::Timestep ts)
{
    NB_PROFILE_FUNCTION();

    m_CameraController.OnUpdate(ts);

    {
        NB_PROFILE_SCOPE("Viewport clear");
        Nebula::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        Nebula::RenderCommand::Clear();
    }

    {
        NB_PROFILE_SCOPE("Render");
        Nebula::Renderer2D::BeginScene(m_CameraController.GetCamera());

        Nebula::Renderer2D::DrawQuad( { .position = {-1.0f, 0.0f, 0.0f}, .rotation = glm::radians(45.0f), .size = {1.0f, 0.8f}, .color = {0.8f, 0.2f, 0.3f, 1.0f} } );
        Nebula::Renderer2D::DrawQuad( { .position = {0.5f, -0.5f, 0.0f}, .rotation = glm::radians(10.0f), .size = {0.5f, 0.8f}, .color = m_SquareColor } );
        Nebula::Renderer2D::DrawQuad( { .position = {1.0f, 0.5f, 0.0f}, .size = {1.0f, 0.8f}, .texture = m_Craft, .color = {0.8f, 1.0f, 1.0f, 1.0f}, .tilingFactor = 10.0f } );

        Nebula::Renderer2D::EndScene();
    }
}

void Sandbox2D::OnImGuiRender()
{
    NB_PROFILE_FUNCTION();

    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

    ImGui::End();
}

void Sandbox2D::OnEvent(Nebula::Event& e)
{
    NB_PROFILE_FUNCTION();

    m_CameraController.OnEvent(e);
}
