#include "EditorLayer.h"

#include <gtc/type_ptr.hpp>
#include <imgui.h>

#include "Nebula/Core/KeyCodes.h"
#include "Nebula.h"

namespace Nebula {
    EditorLayer::EditorLayer()
        : Layer("EditorLayer")
    {
    }

    void EditorLayer::OnAttach()
    {
        m_Framebuffer = Framebuffer::Create({.width = static_cast<uint32_t>(m_ViewportSize.x), .height = static_cast<uint32_t>(m_ViewportSize.y)});

        m_Scene = CreateRef<Scene>();

        m_SquareEntity = m_Scene->CreateEntity("Square");
        m_SquareEntity.AddComponent<SpriteRendererComponent>(glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});

        m_Square2Entity = m_Scene->CreateEntity("Square2");
        m_Square2Entity.AddComponent<SpriteRendererComponent>(glm::vec4{0.0f, 1.0f, 1.0f, 1.0f});
        m_Square2Entity.GetComponent<TransformComponent>().Transform = glm::translate(glm::mat4(1.0f), glm::vec3{-1.0f, 0.0f, -.1f}) * glm::rotate(glm::mat4(1.0f), glm::radians(40.0f), glm::vec3{0.0f, 0.0f, 1.0f});

        m_CameraEntity = m_Scene->CreateEntity("Camera");
        m_CameraEntity.AddComponent<CameraComponent>();

        class CameraController : public ScriptableEntity
        {
        public:
            void OnCreate() {}
            void OnUpdate(Timestep ts)
            {
                auto& transform = GetComponent<TransformComponent>().Transform;

                if (Input::IsKeyPressed(NB_KEY_Q))
                    transform[3][0] -= ts * 2;
                if (Input::IsKeyPressed(NB_KEY_D))
                    transform[3][0] += ts * 2;
                if (Input::IsKeyPressed(NB_KEY_S))
                    transform[3][1] -= ts * 2;
                if (Input::IsKeyPressed(NB_KEY_Z))
                    transform[3][1] += ts * 2;
            }
            void OnDestroy() {}
        };
        m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
    }

    void EditorLayer::OnUpdate(Timestep ts)
    {
        if (!DoesViewportFit) {
            m_Framebuffer->Resize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));

            m_Scene->OnViewportResize(m_ViewportSize.x, m_ViewportSize.y);
        }

        RenderCommand::SetClearColor({ 0.f, 0.f, 0.f, 1.f });
        RenderCommand::Clear();

        m_Framebuffer->Bind();

        RenderCommand::SetClearColor({ .3f, .3f, .3f, 1.f });
        RenderCommand::Clear();

        m_Scene->OnUpdate(ts);

        m_Framebuffer->Unbind();
    }

    void EditorLayer::OnEvent(Event &event)
    {
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
            if (m_ViewportSize != *reinterpret_cast<glm::vec2*>(&size)) {
                m_ViewportSize = { size.x, size.y };
                DoesViewportFit = false;
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
            if (m_CameraEntity)
            {
                ImGui::SeparatorText(std::string(m_CameraEntity.GetComponent<TagComponent>().Tag).c_str());

                auto& cameraTransform = m_CameraEntity.GetComponent<TransformComponent>().Transform;
                ImGui::DragFloat3("Camera Position", glm::value_ptr(cameraTransform[3]));

                auto& camera = m_CameraEntity.GetComponent<CameraComponent>().Camera;
                float orthoSize = camera.GetOrthographicSize();
                if (ImGui::DragFloat("Camera Zoom", &orthoSize))
                {
                    camera.SetOrthographicSize(orthoSize);
                }
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
