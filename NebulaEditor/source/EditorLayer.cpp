#include "EditorLayer.h"

#include <imgui.h>
#include <gtc/type_ptr.hpp>

#include "imgui_internal.h"
#include "Nebula.h"
#include "Nebula/CoreSystems/Application/Application.h"
#include "Nebula/Renderer/Buffers/Framebuffer.h"
#include "Nebula/Renderer/Core/RenderCommand.h"
#include "Nebula/Scene/SceneSerializer.h"
#include "Platform/Nebula/CoreSystems/Windows/WindowManager.h"
#include "Platform/Nebula/Dialogs/FileDialogs.h"

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

        Platform::WindowManager::Get().GetMainWindow()->SetVSync(Platform::VSyncMode::Adaptive);
        Platform::WindowManager::Get().GetMainWindow()->SetWindowHitTest([&](int x, int y)
        {
            ImGuiIO& io = ImGui::GetIO();

            if (y <= Platform::WindowManager::Get().GetMainWindow()->GetTitlebarHeight())
            {
                for (auto& button : m_TitleBarButtons)
                {
                    if (x >= button.x && x <= button.z && y >= button.y && y <= button.w) return Platform::HitTest::Client;
                }
                return Platform::HitTest::TitleBar;
            }

            return Platform::HitTest::Client;
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

    void EditorLayer::OnEvent(Platform::Event &event)
    {
        Platform::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<Platform::KeyPressedEvent>(NB_BIND_EVENT_FN(EditorLayer::OnKeyPressedEvent));
    }

    void EditorLayer::OnImGuiRender()
    {
        m_TitleBarButtons = {};

        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowMinSize.x = 400.0f;

        int32_t height = Platform::WindowManager::Get().GetMainWindow()->GetTitlebarHeight();
        height -= GImGui->FontSize;
        height /= 2;
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0, static_cast<float>(height)});
        ImGui::BeginMainMenuBar();
        {
            ImGui::BeginMenu("NebulaEditor", false);

            std::pair<int32_t, int32_t> window_pos = { 0, 0 };
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) window_pos = Platform::WindowManager::Get().GetMainWindow()->GetClientPos();

            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New Scene", "Ctrl+N")) NewScene();
                if (ImGui::MenuItem("Open Scene...", "Ctrl+O")) OpenScene();
                if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S")) SaveSceneAs();

                ImGui::Separator();

                if (ImGui::MenuItem("Exit", "Alt + F4")) Application::Get().Shutdown();

                ImGui::EndMenu();
            }
            glm::vec4 rect = {ImGui::GetItemRectMin().x - window_pos.first, ImGui::GetItemRectMin().y - window_pos.second, ImGui::GetItemRectMax().x - window_pos.first, ImGui::GetItemRectMax().y - window_pos.second};
            m_TitleBarButtons.emplace_back(rect);

#ifndef NB_PLATFORM_WINDOWS
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 68);
            if (ImGui::MenuItem("_"))
            {
                Platform::WindowManager::Get().GetMainWindow()->Minimize();
            }
            rect = {ImGui::GetItemRectMin().x - window_pos.first, ImGui::GetItemRectMin().y - window_pos.second, ImGui::GetItemRectMax().x - window_pos.first, ImGui::GetItemRectMax().y - window_pos.second};
            m_TitleBarButtons.emplace_back(rect);

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 42);
            if (ImGui::MenuItem("O")) {
                if (Platform::WindowManager::Get().GetMainWindow()->IsMaximized()) Platform::WindowManager::Get().GetMainWindow()->SetMaximized(false);
                else Platform::WindowManager::Get().GetMainWindow()->SetMaximized(true);
            }
            rect = {ImGui::GetItemRectMin().x - window_pos.first, ImGui::GetItemRectMin().y - window_pos.second, ImGui::GetItemRectMax().x - window_pos.first, ImGui::GetItemRectMax().y - window_pos.second};
            m_TitleBarButtons.emplace_back(rect);

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 16);
            if (ImGui::MenuItem("X")) Application::Get().Shutdown();
            rect = {ImGui::GetItemRectMin().x - window_pos.first, ImGui::GetItemRectMin().y - window_pos.second, ImGui::GetItemRectMax().x - window_pos.first, ImGui::GetItemRectMax().y - window_pos.second};
            m_TitleBarButtons.emplace_back(rect);
#endif
        }
        ImGui::EndMainMenuBar();
        ImGui::PopStyleVar();

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

        if (IsFirstRender)
        {
            Platform::WindowManager::Get().GetMainWindow()->RecalculateHitTest();
            IsFirstRender = false;
        }

        // TODO: Add Standard Debug Window
    }

    void EditorLayer::NewScene()
    {
        m_Scene = CreateRef<Scene>();
        m_Scene->OnViewportResize(m_ViewportSize.x, m_ViewportSize.y);
        m_SceneHierarchyPanel.SetContext(m_Scene);
    }

    void EditorLayer::OpenScene()
    {
        /*
        std::string path = Platform::FileDialogs::OpenFile({ Platform::FileType::NebulaScene });
        if (!path.empty()) {
            NewScene();

            SceneSerializer serializer(m_Scene);
            serializer.Deserialize(path);
        }
        */
        Platform::FileDialogs::OpenFile(Platform::FileType::NebulaScene, [&](std::string path)
        {
            NewScene();

            SceneSerializer serializer(m_Scene);
            serializer.Deserialize(path);
        });
    }

    void EditorLayer::SaveSceneAs()
    {
        /*
        std::string path = Platform::FileDialogs::SaveFile("Nebula Scene (*.nbscene)\0*.nbscene\0");
        if (!path.empty()) {
            SceneSerializer serializer(m_Scene);
            serializer.Serialize(path);
        }
        */
        Platform::FileDialogs::SaveFile(Platform::FileType::NebulaScene, [&](std::string path)
        {
            SceneSerializer serializer(m_Scene);
            serializer.Serialize(path);
        });
    }

    bool EditorLayer::OnKeyPressedEvent(const Platform::KeyPressedEvent& event)
    {
        // Shortcuts
        if (event.GetRepeatCount() > 0) return false;
        bool control = Platform::Input::IsKeyPressed(KeyboardKey::LeftControl) || Platform::Input::IsKeyPressed(KeyboardKey::RightControl);
        bool shift = Platform::Input::IsKeyPressed(KeyboardKey::LeftShift) || Platform::Input::IsKeyPressed(KeyboardKey::RightShift);

        if (event.GetKeyCode() == KeyboardKey::N && control) NewScene();
        if (event.GetKeyCode() == KeyboardKey::O && control) OpenScene();
        if (event.GetKeyCode() == KeyboardKey::S && control && shift) SaveSceneAs();
        return false;
    }
}
