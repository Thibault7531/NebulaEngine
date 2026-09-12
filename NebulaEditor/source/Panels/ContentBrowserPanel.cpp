#include "ContentBrowserPanel.h"

#include <filesystem>

#include "imgui.h"

namespace Nebula
{
    void ContentBrowserPanel::OnImGuiRender()
    {
        ImGui::Begin("Content browser");

        if (m_CurrentDirectory.has_parent_path() && ImGui::Button("<-")) m_CurrentDirectory = m_CurrentDirectory.parent_path();

        for (auto& dirEntry : std::filesystem::directory_iterator{m_CurrentDirectory})
        {
            if (dirEntry.is_directory())
            {
                if (ImGui::Button(dirEntry.path().filename().string().c_str())) m_CurrentDirectory = dirEntry.path();
            }
            else
            {
                ImGui::Text("%s", dirEntry.path().filename().string().c_str());
            }
        }

        ImGui::End();
    }
}
