#include "EditorLayer.h"

#include <gtc/type_ptr.hpp>
#include <imgui.h>

#include "Nebula/Core/Application.h"
#include "Nebula/Core/KeyCodes.h"
#include "Nebula/Renderer/RenderCommand.h"
#include "Nebula/Renderer/Renderer2D.h"

namespace Nebula {
    EditorLayer::EditorLayer()
        : Layer("EditorLayer"), m_Camera(16.0f/9.0f)
    {
    }

    void EditorLayer::OnAttach()
    {
        m_Framebuffer = Framebuffer::Create({.width = static_cast<uint32_t>(m_ViewportSize.x), .height = static_cast<uint32_t>(m_ViewportSize.y)});

        m_Scene = CreateRef<Scene>();

        m_SquareEntity = m_Scene->CreateEntity("Square");
        m_SquareEntity.AddComponent<SpriteRendererComponent>(glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});
    }

    void EditorLayer::OnUpdate(Timestep ts)
    {
        if (IsViewportFocused) m_Camera.OnUpdate(ts);

        RenderCommand::SetClearColor({ 0.f, 0.f, 0.f, 1.f });
        RenderCommand::Clear();

        m_Framebuffer->Bind();

        RenderCommand::SetClearColor({ .3f, .3f, .3f, 1.f });
        RenderCommand::Clear();

        {
            Renderer2D::BeginScene(m_Camera.GetCamera());

            Renderer2D::DrawQuad({ .position = {-.5f, .2f, -.1f}, .rotation = glm::radians(40.f), .color = {.0f, 1.f, 1.f, 1.f} });
            m_Scene->OnUpdate(ts);

            Renderer2D::EndScene();
        }

        m_Framebuffer->Unbind();
    }

    void EditorLayer::OnEvent(Event &event)
    {
        if (IsViewportFocused) m_Camera.OnEvent(event);

        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(NB_BIND_EVENT_FN(EditorLayer::OnKeyPressedEvent));
    }

    void EditorLayer::OnImGuiRender()
    {
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
        ImGui::BeginMainMenuBar();
            ImGui::BeginMenu("NebulaEditor", false);
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Exit", "Alt + F4")) Application::Get().Shutdown();
                ImGui::EndMenu();
            }
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 42);
            if (ImGui::MenuItem("o")) {
                if (Application::Get().GetWindow().IsFullscreen()) Application::Get().GetWindow().SetFullscreen(false);
                else Application::Get().GetWindow().SetFullscreen(true);
            };
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 16);
            if (ImGui::MenuItem("x")) Application::Get().Shutdown();
        ImGui::EndMainMenuBar();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
        ImGui::Begin("Scene");
        {
            ImVec2 size = ImGui::GetContentRegionAvail();
            if (m_ViewportSize != *(glm::vec2*)&size) {
                m_ViewportSize = { size.x, size.y };
                m_Framebuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);
                m_Camera.OnResize(m_ViewportSize.x, m_ViewportSize.y);
            }

            IsViewportFocused = ImGui::IsWindowFocused();
            IsViewportHovered = ImGui::IsWindowHovered();
            Application::Get().GetImGuiLayer()->BlockEvents(!IsViewportFocused || !IsViewportHovered);

            ImGui::Image(m_Framebuffer->GetColorAttachmentRendererID(), { m_ViewportSize.x, m_ViewportSize.y }, {0, 1}, {1, 0});
        }
        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::Begin("Properties");
        {
            if (m_SquareEntity)
            {
                ImGui::SeparatorText(std::string(m_SquareEntity.GetComponent<TagComponent>().Tag).c_str());

                auto& SquareColor = m_SquareEntity.GetComponent<SpriteRendererComponent>().Color;
                ImGui::ColorEdit4("Square Color", glm::value_ptr(SquareColor));
            }
        }
        ImGui::End();
    }

    bool EditorLayer::OnKeyPressedEvent(const KeyPressedEvent& event)
    {
        if (event.GetKeyCode() == NB_KEY_F) {
            if (Application::Get().GetWindow().IsFullscreen()) Application::Get().GetWindow().SetFullscreen(false);
            else Application::Get().GetWindow().SetFullscreen(true);
            return true;
        }
        return false;
    }
}
