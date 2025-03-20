#include "EditorLayer.h"

#include "imgui.h"
#include "Nebula/Core/Application.h"
#include "Nebula/Renderer/RenderCommand.h"

namespace Nebula {
    void EditorLayer::OnUpdate(Timestep ts)
    {
        RenderCommand::SetClearColor({ 0.f, 0.f, 0.f, 1.f });
        RenderCommand::Clear();
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
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 16);
        if (ImGui::MenuItem("x")) Application::Get().Shutdown();
        ImGui::EndMainMenuBar();
    }

}
