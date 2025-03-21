#include "EditorLayer.h"

#include "imgui.h"
#include "Nebula/Core/Application.h"
#include "Nebula/Core/KeyCodes.h"
#include "Nebula/Renderer/RenderCommand.h"

namespace Nebula {
    void EditorLayer::OnUpdate(Timestep ts)
    {
        RenderCommand::SetClearColor({ 0.f, 0.f, 0.f, 1.f });
        RenderCommand::Clear();
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
