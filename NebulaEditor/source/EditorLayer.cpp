#include "EditorLayer.h"

#include <imgui_internal.h>
#include <gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

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
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 64);

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

#ifdef NB_PLATFORM_LINUX
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
            ImGuizmo::SetDrawlist();

            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImVec2 size = ImGui::GetContentRegionAvail();
            if (m_ViewportSize != *reinterpret_cast<glm::vec2*>(&size)) {
                m_ViewportSize = { size.x, size.y };
                DoesViewportFit = false;
            }

            IsViewportFocused = ImGui::IsWindowFocused();
            IsViewportHovered = ImGui::IsWindowHovered();
            Application::Get().GetImGuiLayer()->BlockEvents(!IsViewportFocused && !IsViewportHovered);

            ImGui::Image(m_Framebuffer->GetColorAttachmentRendererID(), { m_ViewportSize.x, m_ViewportSize.y }, {0, 1}, {1, 0});

            ImGuizmo::Enable(true);
            ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);
            Entity cameraEntity = m_Scene->GetPrimaryCameraEntity();
            Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
            if (cameraEntity)
            {
                SceneCamera::ProjectionType type = cameraEntity.GetComponent<CameraComponent>().Camera.GetProjectionType();
                ImGuizmo::SetOrthographic(type == SceneCamera::ProjectionType::Orthographic);
                TransformComponent& cameraTransform = cameraEntity.GetComponent<TransformComponent>();
                glm::mat4 view = glm::inverse(cameraTransform.GetTransform());

                if (type == SceneCamera::ProjectionType::Perspective)
                {
                    ImGuizmo::ViewManipulate(glm::value_ptr(view), 8.f, ImVec2(pos.x + size.x - 200, pos.y), ImVec2(200, 200), 0x10101000);

                    if (ImGuizmo::IsUsingViewManipulate())
                    {
                        glm::quat rotation;
                        glm::vec3 skew{0.0f};
                        glm::vec4 perspective{0.0f, 0.0f, 0.0f, 1.0f};
                        glm::decompose(glm::inverse(view), cameraTransform.Scale, rotation, cameraTransform.Translation, skew, perspective);
                        cameraTransform.Rotation = glm::eulerAngles(rotation);
                    }
                }

                if (selectedEntity && selectedEntity != cameraEntity && m_GizmoOperation != -1)
                {
                    glm::mat4 projection = cameraEntity.GetComponent<CameraComponent>().Camera.getProjectionMatrix();
                    glm::mat4 transform = selectedEntity.GetComponent<TransformComponent>().GetTransform();
                    ImGuizmo::PushID(0);
                    ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), static_cast<ImGuizmo::OPERATION>(m_GizmoOperation), ImGuizmo::MODE::WORLD, glm::value_ptr(transform));
                    ImGuizmo::PopID();

                    if (ImGuizmo::IsUsing())
                    {
                        glm::quat rotation;
                        glm::vec3 skew{0.0f};
                        glm::vec4 perspective{0.0f, 0.0f, 0.0f, 1.0f};
                        glm::decompose(transform, selectedEntity.GetComponent<TransformComponent>().Scale, rotation, selectedEntity.GetComponent<TransformComponent>().Translation, skew, perspective);
                        selectedEntity.GetComponent<TransformComponent>().Rotation = glm::eulerAngles(rotation);
                    }
                }
            }
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

        if (event.GetKeyCode() == KeyboardKey::Q) m_GizmoOperation = -1;
        if (event.GetKeyCode() == KeyboardKey::W) m_GizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
        if (event.GetKeyCode() == KeyboardKey::E) m_GizmoOperation = ImGuizmo::OPERATION::ROTATE;
        if (event.GetKeyCode() == KeyboardKey::R) m_GizmoOperation = ImGuizmo::OPERATION::SCALE;
        return false;
    }
}
