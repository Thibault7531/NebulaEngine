#include "Nebula/Debug/ImGuiDebugWindow.h"

#include "imgui.h"
#include "Nebula/CoreSystems/Core/Core.h"
#include "Nebula/Graphics/Core/RendererAPI.h"
#include "Platform/Nebula/CoreSystems/Core/PlatformUtils.h"
#include "Platform/Nebula/CoreSystems/Windows/WindowManager.h"

namespace Nebula::Debug
{
    void ShowDebugWindow()
    {
        ImGui::Begin(("Debug Window"));
        {
            ImGui::BeginTabBar("DebugWindowTabs");
            {
                if (ImGui::BeginTabItem("Windows"))
                {
                    for (const Ref<Platform::Window>& window : Platform::WindowManager::Get())
                    {
                        ImGui::BeginTable("Window Information", 2, ImGuiTableFlags_SizingFixedFit);
                        {
                            ImGui::TableSetupColumn("Window", ImGuiTableColumnFlags_WidthFixed);
                            ImGui::TableSetupColumn("Info");

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Title:");
                            ImGui::TableNextColumn();
                            ImGui::Text("%s", window->GetTitle().c_str());

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Size:");
                            ImGui::TableNextColumn();
                            ImGui::Text("%d x %d", window->GetSize().first, window->GetSize().second);

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Client Size:");
                            ImGui::TableNextColumn();
                            ImGui::Text("%d x %d", window->GetClientSize().first, window->GetClientSize().second);

#ifdef NB_PLATFORM_LINUX
                            ImGui::TableNextRow();
                            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 165, 0, 255));
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Warning:");
                            ImGui::TableNextColumn();
                            ImGui::Text("Window and client positions may be wrong on Linux when using Wayland");
                            ImGui::PopStyleColor();
                            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 100, 100, 255));
#endif

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Position:");
                            ImGui::TableNextColumn();
                            ImGui::Text("%d x %d", window->GetPos().first, window->GetPos().second);

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Client Position:");
                            ImGui::TableNextColumn();
                            ImGui::Text("%d x %d", window->GetClientPos().first, window->GetClientPos().second);

#ifdef NB_PLATFORM_LINUX
                            ImGui::PopStyleColor();
                            ImGui::TableNextRow();
#endif

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Window Style:");
                            ImGui::TableNextColumn();
                            ImGui::Text("%s", Platform::Window::GetWindowStyleName(window->GetProperties().Style).c_str());

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Fullscreen:");
                            ImGui::TableNextColumn();
                            ImGui::Text("%d", window->IsFullscreen());

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Maximized:");
                            ImGui::TableNextColumn();
                            ImGui::Text("%d", window->IsMaximized());

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Minimized:");
                            ImGui::TableNextColumn();
                            ImGui::Text("%d", window->IsMinimized());

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Focused:");
                            ImGui::TableNextColumn();
                            ImGui::Text("%d", window->IsFocused());

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Graphics Surface:");
                            ImGui::TableNextColumn();
                            ImGui::Text("%s", Graphics::RendererAPI::GraphicsAPIStr(window->GetProperties().API).c_str());

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Graphics API Version:");
                            ImGui::TableNextColumn();
                            ImGui::Text("%s", window->GetSurface()->GetAPIVersion().c_str());

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Graphics API Vendor:");
                            ImGui::TableNextColumn();
                            ImGui::Text("%s", window->GetSurface()->GetAPIVendor().c_str());

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Graphics API Renderer:");
                            ImGui::TableNextColumn();
                            ImGui::Text("%s", window->GetSurface()->GetAPIRenderer().c_str());

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("VSync Mode:");
                            ImGui::TableNextColumn();
                            ImGui::Text("%s", window->GetSurface()->GetVSyncModeString().c_str());
                        }
                        ImGui::EndTable();
                        ImGui::Separator();
                    }

                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }
        ImGui::End();
    }
}
