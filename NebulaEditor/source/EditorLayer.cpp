#include "EditorLayer.h"

#include <gtc/type_ptr.hpp>
#include <imgui.h>

#include "imgui_internal.h"
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
        m_Square2Entity.GetComponent<TransformComponent>() = { glm::vec3{-1.0f, 0.0f, -.1f}, glm::vec3{0.f, 0.f, glm::radians(40.0f)}, glm::vec3{1.0f} };

        m_CameraEntity = m_Scene->CreateEntity("Camera");
        m_CameraEntity.AddComponent<CameraComponent>();

        class CameraController : public ScriptableEntity
        {
        public:
            virtual void OnCreate() override {}
            virtual void OnUpdate(Timestep ts) override
            {
                auto& tc = GetComponent<TransformComponent>();

                if (Input::IsKeyPressed(NB_KEY_Q))
                    tc.Translation.x -= ts * 2;
                if (Input::IsKeyPressed(NB_KEY_D))
                    tc.Translation.x += ts * 2;
                if (Input::IsKeyPressed(NB_KEY_S))
                    tc.Translation.y -= ts * 2;
                if (Input::IsKeyPressed(NB_KEY_Z))
                    tc.Translation.y += ts * 2;
            }
            virtual void OnDestroy() override {}
        };
        m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();

        m_SceneHierarchyPanel.SetContext(m_Scene);

        Application::Get().GetImGuiLayer()->BlockEvents(false);

        Application::Get().GetWindow().SetWindowHitTest([&](int x, int y)
        {
            ImGuiIO& io = ImGui::GetIO();

            if (ImGui::GetMousePos().y >= Application::Get().GetWindow().GetPosY() & ImGui::GetMousePos().y <= Application::Get().GetWindow().GetPosY() + 5)
            {
                if (ImGui::GetMousePos().x >= Application::Get().GetWindow().GetPosX() & ImGui::GetMousePos().x <= Application::Get().GetWindow().GetPosX() + 5) return Window::HitTest::ResizeTopLeft;
                if (ImGui::GetMousePos().x >= Application::Get().GetWindow().GetPosX() + 5 & ImGui::GetMousePos().x <= Application::Get().GetWindow().GetPosX() + io.DisplaySize.x - 5) return Window::HitTest::ResizeTop;
                if (ImGui::GetMousePos().x >= Application::Get().GetWindow().GetPosX() + io.DisplaySize.x - 5 & ImGui::GetMousePos().x <= Application::Get().GetWindow().GetPosX() + io.DisplaySize.x) return Window::HitTest::ResizeTopRight;
            }
            if (ImGui::GetMousePos().y >= Application::Get().GetWindow().GetPosY() + 5 & ImGui::GetMousePos().y <= Application::Get().GetWindow().GetPosY() + io.DisplaySize.y - 5)
            {
                if (ImGui::GetMousePos().x >= Application::Get().GetWindow().GetPosX() & ImGui::GetMousePos().x <= Application::Get().GetWindow().GetPosX() + 5) return Window::HitTest::ResizeLeft;
                if (ImGui::GetMousePos().x >= Application::Get().GetWindow().GetPosX() + io.DisplaySize.x - 5 & ImGui::GetMousePos().x <= Application::Get().GetWindow().GetPosX() + io.DisplaySize.x) return Window::HitTest::ResizeRight;
            }
            if (ImGui::GetMousePos().y >= Application::Get().GetWindow().GetPosY() + io.DisplaySize.y - 5 & ImGui::GetMousePos().y <= Application::Get().GetWindow().GetPosY() + io.DisplaySize.y)
            {
                if (ImGui::GetMousePos().x >= Application::Get().GetWindow().GetPosX() & ImGui::GetMousePos().x <= Application::Get().GetWindow().GetPosX() + 5) return Window::HitTest::ResizeBottomLeft;
                if (ImGui::GetMousePos().x >= Application::Get().GetWindow().GetPosX() + 5 & ImGui::GetMousePos().x <= Application::Get().GetWindow().GetPosX() + io.DisplaySize.x - 5) return Window::HitTest::ResizeBottom;
                if (ImGui::GetMousePos().x >= Application::Get().GetWindow().GetPosX() + io.DisplaySize.x - 5 & ImGui::GetMousePos().x <= Application::Get().GetWindow().GetPosX() + io.DisplaySize.x) return Window::HitTest::ResizeBottomRight;
            }

            if (IsTitleBarHovered) return Window::HitTest::Draggable;
            return Window::HitTest::None;
        });

        NB_INFO("Nebula Editor Loaded");
    }

    void EditorLayer::OnUpdate(Timestep ts)
    {
        if (!DoesViewportFit) {
            m_Framebuffer->Resize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));

            m_Scene->OnViewportResize(m_ViewportSize.x, m_ViewportSize.y);

            DoesViewportFit = true;
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

        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowMinSize.x = 400.0f;

        ImGui::BeginMainMenuBar();
        {
            IsTitleBarHovered = ImGui::IsWindowHovered();

            ImGui::BeginMenu("NebulaEditor", false);
            IsTitleBarHovered &= !ImGui::IsItemHovered();

            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Exit", "Alt + F4")) Application::Get().Shutdown();
                ImGui::EndMenu();
            }
            IsTitleBarHovered &= !ImGui::IsItemHovered();

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 68);
            if (ImGui::MenuItem("_"))
            {
                Application::Get().GetWindow().Minimize();
            }
            IsTitleBarHovered &= !ImGui::IsItemHovered();

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 42);
            if (ImGui::MenuItem("O")) {
                if (Application::Get().GetWindow().IsMaximized()) Application::Get().GetWindow().SetMaximized(false);
                else Application::Get().GetWindow().SetMaximized(true);
            }
            IsTitleBarHovered &= !ImGui::IsItemHovered();

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 16);
            if (ImGui::MenuItem("X")) Application::Get().Shutdown();
            IsTitleBarHovered &= !ImGui::IsItemHovered();
        }
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

        m_SceneHierarchyPanel.OnImGuiRender();
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
