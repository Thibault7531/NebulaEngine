#include "EditorLayer.h"

#include <imgui_internal.h>
#include <gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <nanosvg.h>
#define NANOSVGRAST_IMPLEMENTATION
#include <nanosvgrast.h>

#include "Nebula.h"
#include "Nebula/CoreSystems/Application/Application.h"
#include "Nebula/Debug/ImGuiDebugWindow.h"
#include "Nebula/Graphics/Core/RendererAPI.h"
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

        m_Device = Application::Get().GetDevice();

        m_Framebuffer = m_Device->CreateFramebuffer({.width = static_cast<uint32_t>(m_ViewportSize.x), .height = static_cast<uint32_t>(m_ViewportSize.y), .AttachmentSpec = {Graphics::Texture::InternalFormat::RGBA8, Graphics::Texture::InternalFormat::R32I, Graphics::Texture::InternalFormat::Depth24Stencil8}});
        m_Framebuffer->SetClearColors({{.1f, .1f, .1f, 1.f}, glm::vec4{-1.f}});

        m_Scene = CreateRef<Scene>();

        m_Camera = EditorCamera(30.f, m_ViewportSize.x / m_ViewportSize.y, 0.1f, 1000.f);

        m_SceneHierarchyPanel.SetContext(m_Scene);
        m_ContentBrowserPanel.SetOpenSceneCallback([&](std::filesystem::path path)
        {
            OpenScene(path);
        });

        Platform::WindowManager::Get().GetMainWindow()->SetVSync(Graphics::VSyncMode::Adaptive);
        Platform::WindowManager::Get().GetMainWindow()->SetWindowHitTest([&](int x, int y)
        {
            ImGuiIO& io = ImGui::GetIO();

            auto [WindowWidth, WindowHeight] = Platform::WindowManager::Get().GetMainWindow()->GetSize();

            if (y >= 0 && y <= 5)
            {
                if (x >= 0 && x <= 5) return Platform::HitTest::ResizeTopLeft;
                if (x > 5 && x < WindowWidth - 5) return Platform::HitTest::ResizeTop;
                if (x >= WindowWidth - 5 && x <= WindowWidth) return Platform::HitTest::ResizeTopRight;
            }
            if (y > 5 && y < WindowHeight - 5)
            {
                if (x >= 0 && x <= 5) return Platform::HitTest::ResizeLeft;
                if (x >= WindowWidth - 5 && x <= WindowWidth) return Platform::HitTest::ResizeRight;
            }
            if (y >= WindowHeight - 5 && y <= WindowHeight)
            {
                if (x >= 0 && x <= 5) return Platform::HitTest::ResizeBottomLeft;
                if (x > 5 && x < WindowWidth - 5) return Platform::HitTest::ResizeBottom;
                if (x >= WindowWidth - 5 && x <= WindowWidth) return Platform::HitTest::ResizeBottomRight;
            }

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

        uint32_t imageSize = 16;
        NSVGrasterizer* rast = nsvgCreateRasterizer();

        NSVGimage* closeImage = nsvgParseFromFile((Platform::Files::GetResourcesPath() / "assets" / "NebulaEditor" / "icons" / "window-close-symbolic.svg").string().c_str(), "px", 96);
        auto* pixels = static_cast<unsigned char*>(malloc(imageSize * imageSize * 4));
        nsvgRasterize(rast, closeImage, 0, 0, imageSize/closeImage->width, pixels, imageSize, imageSize, imageSize * 4);
        m_CloseIcon = m_Device->CreateTexture2D(imageSize, imageSize, Graphics::Texture::InternalFormat::RGBA8);
        m_CloseIcon->SetData(pixels, imageSize * imageSize * 4, Graphics::Texture::DataFormat::RGBA);
        free(pixels);
        nsvgDelete(closeImage);

        NSVGimage* maximizeImage = nsvgParseFromFile((Platform::Files::GetResourcesPath() / "assets" / "NebulaEditor" / "icons" / "window-maximize-symbolic.svg").string().c_str(), "px", 96);
        pixels = static_cast<unsigned char*>(malloc(imageSize * imageSize * 4));
        nsvgRasterize(rast, maximizeImage, 0, 0, imageSize/maximizeImage->width, pixels, imageSize, imageSize, imageSize * 4);
        m_MaximizeIcon = m_Device->CreateTexture2D(imageSize, imageSize, Graphics::Texture::InternalFormat::RGBA8);
        m_MaximizeIcon->SetData(pixels, imageSize * imageSize * 4, Graphics::Texture::DataFormat::RGBA);
        free(pixels);
        nsvgDelete(maximizeImage);

        NSVGimage* restoreImage = nsvgParseFromFile((Platform::Files::GetResourcesPath() / "assets" / "NebulaEditor" / "icons" / "window-restore-symbolic.svg").string().c_str(), "px", 96);
        pixels = static_cast<unsigned char*>(malloc(imageSize * imageSize * 4));
        nsvgRasterize(rast, restoreImage, 0, 0, imageSize/restoreImage->width, pixels, imageSize, imageSize, imageSize * 4);
        m_RestoreIcon = m_Device->CreateTexture2D(imageSize, imageSize, Graphics::Texture::InternalFormat::RGBA8);
        m_RestoreIcon->SetData(pixels, imageSize * imageSize * 4, Graphics::Texture::DataFormat::RGBA);
        free(pixels);
        nsvgDelete(restoreImage);

        NSVGimage* minimizeImage = nsvgParseFromFile((Platform::Files::GetResourcesPath() / "assets" / "NebulaEditor" / "icons" / "window-minimize-symbolic.svg").string().c_str(), "px", 96);
        pixels = static_cast<unsigned char*>(malloc(imageSize * imageSize * 4));
        nsvgRasterize(rast, minimizeImage, 0, 0, imageSize/minimizeImage->width, pixels, imageSize, imageSize, imageSize * 4);
        m_MinimizeIcon = m_Device->CreateTexture2D(imageSize, imageSize, Graphics::Texture::InternalFormat::RGBA8);
        m_MinimizeIcon->SetData(pixels, imageSize * imageSize * 4, Graphics::Texture::DataFormat::RGBA);
        free(pixels);
        nsvgDelete(minimizeImage);

        nsvgDeleteRasterizer(rast);

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
            m_Camera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);

            DoesViewportFit = true;
        }

        Ref<Graphics::CommandBuffer> commandBuffer = m_Device->CreateCommandBuffer();
        Ref<Graphics::RenderPass> renderPass = commandBuffer->CreateRenderPass(Application::Get().GetMainFramebuffer(), true);
        renderPass->EndRenderPass();
        commandBuffer->Commit();

        SceneRenderer::RenderSceneEditor(m_Scene, m_Camera, m_Framebuffer);
    }

    void EditorLayer::OnEvent(Platform::Event &event)
    {
        m_Camera.OnEvent(event);

        Platform::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<Platform::KeyPressedEvent>(NB_BIND_EVENT_FN(EditorLayer::OnKeyPressedEvent));
        dispatcher.Dispatch<Platform::MouseButtonPressedEvent>(NB_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressedEvent));
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
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
        ImGui::BeginMainMenuBar();
        ImGui::PopStyleVar();
        {
#ifdef NB_PLATFORM_MACOS
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 64);
#endif

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

            if (ImGui::BeginMenu("Theme"))
            {
                if (ImGui::MenuItem("Dark", 0, ImGuiLayer::s_ImGuiTheme == Platform::Theme::ThemeType::Dark && !ImGuiLayer::s_AutoTheme))
                {
                    ImGuiLayer::SetDarkTheme();
                    ImGuiLayer::s_AutoTheme = false;
                }
                if (ImGui::MenuItem("Light", 0, ImGuiLayer::s_ImGuiTheme == Platform::Theme::ThemeType::Light && !ImGuiLayer::s_AutoTheme))
                {
                    ImGuiLayer::SetLightTheme();
                    ImGuiLayer::s_AutoTheme = false;
                }
                if (ImGui::MenuItem("Auto", 0, ImGuiLayer::s_AutoTheme))
                {
                    ImGuiLayer::s_AutoTheme = true;
                }
                ImGui::EndMenu();
            }
            rect = {ImGui::GetItemRectMin().x - window_pos.first, ImGui::GetItemRectMin().y - window_pos.second, ImGui::GetItemRectMax().x - window_pos.first, ImGui::GetItemRectMax().y - window_pos.second};
            m_TitleBarButtons.emplace_back(rect);

            if (ImGui::BeginMenu("Debug"))
            {
                if (ImGui::MenuItem("Show Nebula Debug Window", 0, m_ShowNebulaDebugWindow)) m_ShowNebulaDebugWindow = !m_ShowNebulaDebugWindow;
                if (ImGui::MenuItem("Show ImGui Demo Window", 0, m_ShowImGuiDemoWindow)) m_ShowImGuiDemoWindow = !m_ShowImGuiDemoWindow;
                ImGui::EndMenu();
            }
            rect = {ImGui::GetItemRectMin().x - window_pos.first, ImGui::GetItemRectMin().y - window_pos.second, ImGui::GetItemRectMax().x - window_pos.first, ImGui::GetItemRectMax().y - window_pos.second};
            m_TitleBarButtons.emplace_back(rect);

#ifdef NB_PLATFORM_LINUX
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, 0});
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

            float titlabarHeight = Platform::WindowManager::Get().GetMainWindow()->GetTitlebarHeight();
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            int buttonAlpha = 25;
            uint32_t buttonSize = 22;
            uint32_t buttonSpacing = 5;
            int buttonColor = ImGuiLayer::s_ImGuiTheme == Platform::Theme::ThemeType::Dark ? 255 : 0;
            uint32_t textureSize = 16;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 3*titlabarHeight - 2*buttonSpacing);
            float buttonPos = ImGui::GetCursorPosX();
            if (ImGui::InvisibleButton("_", {titlabarHeight, titlabarHeight}, 0))
            {
                Platform::WindowManager::Get().GetMainWindow()->Minimize();
            }
            rect = {ImGui::GetItemRectMin().x - window_pos.first, ImGui::GetItemRectMin().y - window_pos.second, ImGui::GetItemRectMax().x - window_pos.first, ImGui::GetItemRectMax().y - window_pos.second};
            m_TitleBarButtons.emplace_back(rect);
            if (ImGui::IsItemHovered()) buttonAlpha = 50;
            else buttonAlpha = 25;
            draw_list->AddCircleFilled({ buttonPos + titlabarHeight / 2, titlabarHeight / 2 }, buttonSize / 2, ImColor(buttonColor, buttonColor, buttonColor, buttonAlpha), 16);
            ImGui::SetCursorPos({buttonPos + titlabarHeight / 2 - textureSize / 2, (titlabarHeight - textureSize) /2});
            ImGui::ImageWithBg(m_MinimizeIcon.get(), {static_cast<float>(textureSize), static_cast<float>(textureSize)}, {0, 0}, {1, 1}, {0, 0, 0, 0}, ImColor(buttonColor, buttonColor, buttonColor, 255));

            buttonPos += titlabarHeight + buttonSpacing;
            ImGui::SetCursorPosX(buttonPos);
            if (ImGui::InvisibleButton("O", {titlabarHeight, titlabarHeight}, 0))
            {
                if (Platform::WindowManager::Get().GetMainWindow()->IsMaximized()) Platform::WindowManager::Get().GetMainWindow()->SetMaximized(false);
                else Platform::WindowManager::Get().GetMainWindow()->SetMaximized(true);
            }
            rect = {ImGui::GetItemRectMin().x - window_pos.first, ImGui::GetItemRectMin().y - window_pos.second, ImGui::GetItemRectMax().x - window_pos.first, ImGui::GetItemRectMax().y - window_pos.second};
            m_TitleBarButtons.emplace_back(rect);
            if (ImGui::IsItemHovered()) buttonAlpha = 50;
            else buttonAlpha = 25;
            draw_list->AddCircleFilled({ buttonPos + titlabarHeight / 2, titlabarHeight / 2 }, buttonSize / 2, ImColor(buttonColor, buttonColor, buttonColor, buttonAlpha), 16);
            ImGui::SetCursorPos({buttonPos + titlabarHeight / 2 - textureSize / 2, (titlabarHeight - textureSize) /2});
            if (Platform::WindowManager::Get().GetMainWindow()->IsMaximized()) ImGui::ImageWithBg(m_RestoreIcon.get(), {static_cast<float>(textureSize), static_cast<float>(textureSize)}, {0, 0}, {1, 1}, {0, 0, 0, 0}, ImColor(buttonColor, buttonColor, buttonColor, 255));
            else ImGui::ImageWithBg(m_MaximizeIcon.get(), {static_cast<float>(textureSize), static_cast<float>(textureSize)}, {0, 0}, {1, 1}, {0, 0, 0, 0}, ImColor(buttonColor, buttonColor, buttonColor, 255));

            buttonPos += titlabarHeight + buttonSpacing;
            ImGui::SetCursorPosX(buttonPos);
            if (ImGui::InvisibleButton("X", {titlabarHeight, titlabarHeight}, 0))
            {
                Application::Get().Shutdown();
            }
            rect = {ImGui::GetItemRectMin().x - window_pos.first, ImGui::GetItemRectMin().y - window_pos.second, ImGui::GetItemRectMax().x - window_pos.first, ImGui::GetItemRectMax().y - window_pos.second};
            m_TitleBarButtons.emplace_back(rect);
            if (ImGui::IsItemHovered()) buttonAlpha = 50;
            else buttonAlpha = 25;
            draw_list->AddCircleFilled({ buttonPos + titlabarHeight / 2, titlabarHeight / 2 }, buttonSize / 2, ImColor(buttonColor, buttonColor, buttonColor, buttonAlpha), 16);
            ImGui::SetCursorPos({buttonPos + titlabarHeight / 2 - textureSize / 2, (titlabarHeight - textureSize) /2});
            ImGui::ImageWithBg(m_CloseIcon.get(), {static_cast<float>(textureSize), static_cast<float>(textureSize)}, {0, 0}, {1, 1}, {0, 0, 0, 0}, ImColor(buttonColor, buttonColor, buttonColor, 255));

            ImGui::PopStyleVar();
            ImGui::PopStyleVar();
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
            if (m_ViewportPos != *reinterpret_cast<glm::vec2*>(&pos))
            {
                m_ViewportPos = { pos.x, pos.y };
            }

            IsViewportFocused = ImGui::IsWindowFocused();
            IsViewportHovered = ImGui::IsWindowHovered();
            Application::Get().GetImGuiLayer()->BlockEvents(!IsViewportFocused && !IsViewportHovered);

            ImVec2 uv0 = {0, 0};
            ImVec2 uv1 = {1, 1};
            if (Graphics::RendererAPI::GetAPI() == Platform::GraphicsAPI::OpenGL460)
            {
                uv0 = {0, 1};
                uv1 = {1, 0};
            }
            ImGui::Image(m_Framebuffer->GetColorAttachment().get(), { m_ViewportSize.x, m_ViewportSize.y }, uv0, uv1);

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NBSCENE_DRAG"))
                {
                    OpenScene(static_cast<const char*>(payload->Data));
                }

                ImGui::EndDragDropTarget();
            }

            ImGuizmo::Enable(true);
            ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);
            Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
            ImGuizmo::SetOrthographic(false);
            glm::mat4 view = m_Camera.GetViewMatrix();

            // TODO: Reactivate when ImGuizmo fixes righthandedness
            /*
            ImGuizmo::ViewManipulate(glm::value_ptr(view), m_Camera.GetDistance(), ImVec2(pos.x + size.x - 200, pos.y), ImVec2(200, 200), 0x10101000);

            if (ImGuizmo::IsUsingViewManipulate())
            {
                float yaw = glm::atan(view[0][2], view[2][2]);
                float pitch = glm::asin(-view[1][2]);

                m_Camera.SetYaw(-yaw);
                m_Camera.SetPitch(-pitch);
            }
            */

            if (selectedEntity && m_GizmoOperation != -1)
            {
                glm::mat4 projection = m_Camera.GetProjectionMatrix();
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
        ImGui::End();
        ImGui::PopStyleVar();

        m_SceneHierarchyPanel.OnImGuiRender();
        m_ContentBrowserPanel.OnImGuiRender();

        if (m_ShowNebulaDebugWindow) Debug::ShowDebugWindow();
        if (m_ShowImGuiDemoWindow) ImGui::ShowDemoWindow();

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
        Platform::FileDialogs::OpenFile(Platform::FileType::NebulaScene, [&](std::string path)
        {
            NewScene();

            SceneSerializer serializer(m_Scene);
            serializer.Deserialize(path);
        });
    }

    void EditorLayer::OpenScene(const std::filesystem::path& path)
    {
        NewScene();

        SceneSerializer serializer(m_Scene);
        serializer.Deserialize(path);
    }

    void EditorLayer::SaveSceneAs()
    {
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

    bool EditorLayer::OnMouseButtonPressedEvent(const Platform::MouseButtonPressedEvent& event)
    {
        if (event.GetMouseButton() == MouseButton::Left && IsViewportHovered && !ImGuizmo::IsOver() && !Platform::Input::IsKeyPressed(KeyboardKey::LeftAlt))
        {
            ImVec2 mousePos = ImGui::GetMousePos();
            uint32_t mouse_X = mousePos.x - m_ViewportPos.x;
            uint32_t mouse_Y = mousePos.y - m_ViewportPos.y;

            Ref<Graphics::Texture2D> texture = m_Framebuffer->GetColorAttachment(1);
            std::vector<int32_t> viewport = texture->GetDataR32I();
            int32_t entity = viewport[mouse_Y * texture->GetWidth() + mouse_X];
            m_SceneHierarchyPanel.SetSelectedEntity(entity == -1 ? Entity() : Entity(static_cast<entt::entity>(entity), m_Scene.get()));
        }
        return false;
    }
}
