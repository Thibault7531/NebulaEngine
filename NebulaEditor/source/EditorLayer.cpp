#include "EditorLayer.h"

#include <imgui.h>
#include <gtc/type_ptr.hpp>

#include "imgui_internal.h"
#include "Nebula.h"
#include "Nebula/CoreSystems/Input/KeyCodes.h"
#include "Nebula/CoreSystems/Windows/WindowManager.h"
#include "Nebula/Scene/SceneSerializer.h"
#include "Nebula/Utils/PlatformUtils.h"

namespace Nebula {
    EditorLayer::EditorLayer()
        : Layer("EditorLayer")
    {
    }

    void EditorLayer::OnAttach()
    {
        Application::Get().GetImGuiLayer()->BlockEvents(false);

        m_Framebuffer = Framebuffer::Create({.width = static_cast<uint32_t>(m_ViewportSize.x), .height = static_cast<uint32_t>(m_ViewportSize.y)});

        m_Scene = CreateRef<Scene>();

        m_SceneHierarchyPanel.SetContext(m_Scene);

        Application::Get().GetImGuiLayer()->BlockEvents(false);

        WindowManager::Get().GetMainWindow()->SetWindowHitTest([&](int x, int y)
        {
            ImGuiIO& io = ImGui::GetIO();

            if (ImGui::GetMousePos().y >= WindowManager::Get().GetMainWindow()->GetPos().second & ImGui::GetMousePos().y <= WindowManager::Get().GetMainWindow()->GetPos().second + 5)
            {
                if (ImGui::GetMousePos().x >= WindowManager::Get().GetMainWindow()->GetPos().first & ImGui::GetMousePos().x <= WindowManager::Get().GetMainWindow()->GetPos().first + 5) return Window::HitTest::ResizeTopLeft;
                if (ImGui::GetMousePos().x >= WindowManager::Get().GetMainWindow()->GetPos().first + 5 & ImGui::GetMousePos().x <= WindowManager::Get().GetMainWindow()->GetPos().first + io.DisplaySize.x - 5) return Window::HitTest::ResizeTop;
                if (ImGui::GetMousePos().x >= WindowManager::Get().GetMainWindow()->GetPos().first + io.DisplaySize.x - 5 & ImGui::GetMousePos().x <= WindowManager::Get().GetMainWindow()->GetPos().first + io.DisplaySize.x) return Window::HitTest::ResizeTopRight;
            }
            if (ImGui::GetMousePos().y >= WindowManager::Get().GetMainWindow()->GetPos().second + 5 & ImGui::GetMousePos().y <= WindowManager::Get().GetMainWindow()->GetPos().second + io.DisplaySize.y - 5)
            {
                if (ImGui::GetMousePos().x >= WindowManager::Get().GetMainWindow()->GetPos().first & ImGui::GetMousePos().x <= WindowManager::Get().GetMainWindow()->GetPos().first + 5) return Window::HitTest::ResizeLeft;
                if (ImGui::GetMousePos().x >= WindowManager::Get().GetMainWindow()->GetPos().first + io.DisplaySize.x - 5 & ImGui::GetMousePos().x <= WindowManager::Get().GetMainWindow()->GetPos().first + io.DisplaySize.x) return Window::HitTest::ResizeRight;
            }
            if (ImGui::GetMousePos().y >= WindowManager::Get().GetMainWindow()->GetPos().second + io.DisplaySize.y - 5 & ImGui::GetMousePos().y <= WindowManager::Get().GetMainWindow()->GetPos().second + io.DisplaySize.y)
            {
                if (ImGui::GetMousePos().x >= WindowManager::Get().GetMainWindow()->GetPos().first & ImGui::GetMousePos().x <= WindowManager::Get().GetMainWindow()->GetPos().first + 5) return Window::HitTest::ResizeBottomLeft;
                if (ImGui::GetMousePos().x >= WindowManager::Get().GetMainWindow()->GetPos().first + 5 & ImGui::GetMousePos().x <= WindowManager::Get().GetMainWindow()->GetPos().first + io.DisplaySize.x - 5) return Window::HitTest::ResizeBottom;
                if (ImGui::GetMousePos().x >= WindowManager::Get().GetMainWindow()->GetPos().first + io.DisplaySize.x - 5 & ImGui::GetMousePos().x <= WindowManager::Get().GetMainWindow()->GetPos().first + io.DisplaySize.x) return Window::HitTest::ResizeBottomRight;
            }

            if (IsTitleBarHovered) return Window::HitTest::Draggable;
            return Window::HitTest::None;
        });

        NB_INFO("Nebula Editor Loaded");
    }

    void EditorLayer::OnDetach()
    {
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
                if (ImGui::MenuItem("New Scene", "Ctrl+N")) NewScene();
                if (ImGui::MenuItem("Open Scene...", "Ctrl+O")) OpenScene();
                if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S")) SaveSceneAs();

                ImGui::Separator();

                if (ImGui::MenuItem("Exit", "Alt + F4")) Application::Get().Shutdown();

                ImGui::EndMenu();
            }
            IsTitleBarHovered &= !ImGui::IsItemHovered();

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 68);
            if (ImGui::MenuItem("_"))
            {
                WindowManager::Get().GetMainWindow()->Minimize();
            }
            IsTitleBarHovered &= !ImGui::IsItemHovered();

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 42);
            if (ImGui::MenuItem("O")) {
                if (WindowManager::Get().GetMainWindow()->IsMaximized()) WindowManager::Get().GetMainWindow()->SetMaximized(false);
                else WindowManager::Get().GetMainWindow()->SetMaximized(true);
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

            ImGui::Image(m_Framebuffer->GetColorAttachmentRendererID(), { m_ViewportSize.x, m_ViewportSize.y }, {0, 1}, {1, 0});
        }
        ImGui::End();
        ImGui::PopStyleVar();

        m_SceneHierarchyPanel.OnImGuiRender();
    }

    void EditorLayer::NewScene()
    {
        m_Scene = CreateRef<Scene>();
        m_Scene->OnViewportResize(m_ViewportSize.x, m_ViewportSize.y);
        m_SceneHierarchyPanel.SetContext(m_Scene);
    }

    void EditorLayer::OpenScene()
    {
        std::string path = FileDialogs::OpenFile({ FileDialogs::FileType::NebulaScene });
        if (!path.empty()) {
            NewScene();

            SceneSerializer serializer(m_Scene);
            serializer.Deserialize(path);
        }
    }

    void EditorLayer::SaveSceneAs()
    {
        std::string path = FileDialogs::SaveFile("Nebula Scene (*.nbscene)\0*.nbscene\0");
        if (!path.empty()) {
            SceneSerializer serializer(m_Scene);
            serializer.Serialize(path);
        }
    }

    bool EditorLayer::OnKeyPressedEvent(const KeyPressedEvent& event)
    {
        // Shortcuts
        if (event.GetRepeatCount() > 0) return false;
        bool control = Input::IsKeyPressed(NB_KEY_LCTRL) || Input::IsKeyPressed(NB_KEY_RCTRL);
        bool shift = Input::IsKeyPressed(NB_KEY_LSHIFT) || Input::IsKeyPressed(NB_KEY_RSHIFT);

        if (event.GetKeyCode() == NB_KEY_N && control) NewScene();
        if (event.GetKeyCode() == NB_KEY_O && control) OpenScene();
        if (event.GetKeyCode() == NB_KEY_S && control && shift) SaveSceneAs();
        return false;
    }
}
