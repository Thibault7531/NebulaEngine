#include "Sandbox2D.h"

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
    m_CameraController.OnUpdate(ts);

    Nebula::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
    Nebula::RenderCommand::Clear();

    Nebula::Renderer2D::BeginScene(m_CameraController.GetCamera());

    Nebula::Renderer2D::DrawQuad({-1.0f, 0.0f}, 45.0f, {1.0f, 0.8f}, {0.8f, 0.2f, 0.3f, 1.0f});
    Nebula::Renderer2D::DrawQuad({0.5f, -0.5f}, 10.0f, {0.5f, 0.8f}, m_SquareColor);
    Nebula::Renderer2D::DrawQuad({1.0f, 0.5f}, 0.0f, {1.0f, 0.8f}, {1.0f, 1.0f, 1.0f, 1.0f}, m_Craft);

    Nebula::Renderer2D::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
    ImGui::End();
}

void Sandbox2D::OnEvent(Nebula::Event& e)
{
    m_CameraController.OnEvent(e);
}
