#include "ContentBrowserPanel.h"

#include <filesystem>

#include "imgui.h"
#include "nanosvgrast.h"
#include "Nebula/CoreSystems/Application/Application.h"

namespace Nebula
{
    const std::string s_StartingPath = "assets";

    ContentBrowserPanel::ContentBrowserPanel()
    {
        m_History.push_back(std::filesystem::absolute(s_StartingPath));
        m_CurrentDirectory = std::filesystem::absolute(s_StartingPath);

        m_Device = Application::Get().GetDevice();

        m_IconSize = 16;
        m_ThumbnailSize = 96;
        NSVGrasterizer* rasterizer = nsvgCreateRasterizer();

        NSVGimage* leftArrowImage = nsvgParseFromFile((Platform::Files::GetResourcesPath() / "assets" / "NebulaEditor" / "icons" / "chevron-left.svg").string().c_str(), "px", 96);
        auto* pixels = static_cast<unsigned char*>(malloc(m_IconSize * m_IconSize * 4));
        nsvgRasterize(rasterizer, leftArrowImage, 0, 0, m_IconSize/leftArrowImage->width, pixels, m_IconSize, m_IconSize, m_IconSize * 4);
        m_LeftArrowIcon = m_Device->CreateTexture2D(m_IconSize, m_IconSize, Graphics::Texture::InternalFormat::RGBA8);
        m_LeftArrowIcon->SetData(pixels, m_IconSize * m_IconSize * 4, Graphics::Texture::DataFormat::RGBA);
        free(pixels);
        nsvgDelete(leftArrowImage);

        NSVGimage* rightArrowImage = nsvgParseFromFile((Platform::Files::GetResourcesPath() / "assets" / "NebulaEditor" / "icons" / "chevron-right.svg").string().c_str(), "px", 96);
        pixels = static_cast<unsigned char*>(malloc(m_IconSize * m_IconSize * 4));
        nsvgRasterize(rasterizer, rightArrowImage, 0, 0, m_IconSize/rightArrowImage->width, pixels, m_IconSize, m_IconSize, m_IconSize * 4);
        m_RightArrowIcon = m_Device->CreateTexture2D(m_IconSize, m_IconSize, Graphics::Texture::InternalFormat::RGBA8);
        m_RightArrowIcon->SetData(pixels, m_IconSize * m_IconSize * 4, Graphics::Texture::DataFormat::RGBA);
        free(pixels);
        nsvgDelete(rightArrowImage);

        NSVGimage* upArrowImage = nsvgParseFromFile((Platform::Files::GetResourcesPath() / "assets" / "NebulaEditor" / "icons" / "chevron-up.svg").string().c_str(), "px", 96);
        pixels = static_cast<unsigned char*>(malloc(m_IconSize * m_IconSize * 4));
        nsvgRasterize(rasterizer, upArrowImage, 0, 0, m_IconSize/upArrowImage->width, pixels, m_IconSize, m_IconSize, m_IconSize * 4);
        m_TopArrowIcon = m_Device->CreateTexture2D(m_IconSize, m_IconSize, Graphics::Texture::InternalFormat::RGBA8);
        m_TopArrowIcon->SetData(pixels, m_IconSize * m_IconSize * 4, Graphics::Texture::DataFormat::RGBA);
        free(pixels);
        nsvgDelete(upArrowImage);

        NSVGimage* folderImage = nsvgParseFromFile((Platform::Files::GetResourcesPath() / "assets" / "NebulaEditor" / "icons" / "folder.svg").string().c_str(), "px", 96);
        pixels = static_cast<unsigned char*>(malloc(m_ThumbnailSize * m_ThumbnailSize * 4));
        nsvgRasterize(rasterizer, folderImage, 0, 0, m_ThumbnailSize/folderImage->width, pixels, m_ThumbnailSize, m_ThumbnailSize, m_ThumbnailSize * 4);
        m_FolderIcon = m_Device->CreateTexture2D(m_ThumbnailSize, m_ThumbnailSize, Graphics::Texture::InternalFormat::RGBA8);
        m_FolderIcon->SetData(pixels, m_ThumbnailSize * m_ThumbnailSize * 4, Graphics::Texture::DataFormat::RGBA);
        free(pixels);
        nsvgDelete(folderImage);

        NSVGimage* fileImage = nsvgParseFromFile((Platform::Files::GetResourcesPath() / "assets" / "NebulaEditor" / "icons" / "file.svg").string().c_str(), "px", 96);
        pixels = static_cast<unsigned char*>(malloc(m_ThumbnailSize * m_ThumbnailSize * 4));
        nsvgRasterize(rasterizer, fileImage, 0, 0, m_ThumbnailSize/fileImage->width, pixels, m_ThumbnailSize, m_ThumbnailSize, m_ThumbnailSize * 4);
        m_FileIcon = m_Device->CreateTexture2D(m_ThumbnailSize, m_ThumbnailSize, Graphics::Texture::InternalFormat::RGBA8);
        m_FileIcon->SetData(pixels, m_ThumbnailSize * m_ThumbnailSize * 4, Graphics::Texture::DataFormat::RGBA);
        free(pixels);
        nsvgDelete(fileImage);

        nsvgDeleteRasterizer(rasterizer);
    }

    void ContentBrowserPanel::OnImGuiRender()
    {
        ImGui::Begin("Content browser");

        DisplayNavigationBar();
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.2f, 0.22f, 0.25f, 1.0f));
        ImGui::Separator();
        ImGui::PopStyleColor();
        ImGui::Spacing();

        /*
        for (auto& dirEntry : std::filesystem::directory_iterator{m_CurrentDirectory})
        {
            if (dirEntry.is_directory())
            {
                if (ImGui::Button(dirEntry.path().filename().string().c_str()))
                {
                    if (dirEntry.path() != m_CurrentDirectory)
                    {
                        if (m_CurrentHistoryIndex != m_History.size() - 1)
                        {
                            m_History.erase(m_History.begin() + m_CurrentHistoryIndex + 1, m_History.end());
                        }
                        m_CurrentDirectory = dirEntry.path();
                        m_History.push_back(m_CurrentDirectory);
                        m_CurrentHistoryIndex++;
                    }
                }
            }
            else
            {
                ImGui::Text("%s", dirEntry.path().filename().string().c_str());
            }
        }
        */

        float thumbnailSize = 96.f;
        float padding = 16.f;
        float cellSize = thumbnailSize + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = std::floor(panelWidth / cellSize);
        if (columnCount < 1) columnCount = 1;

        if (ImGui::BeginTable("ContentGrid", columnCount))
        {
            for (auto& dirEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
            {
                ImGui::TableNextColumn();
                DisplayAssetCard(dirEntry, thumbnailSize, padding);
            }
            ImGui::EndTable();

            if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                m_SelectedItem.clear();
            }
        }

        ImGui::End();
    }

    void ContentBrowserPanel::SetNavigationButtonColors()
    {
        if (ImGuiLayer::s_ImGuiTheme == Platform::Theme::ThemeType::Dark)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.10f, 0.1f, 0.1f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.2f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 0.5f));
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 0.2f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.1f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 0.5f));
        }
    }

    void ContentBrowserPanel::ResetNavigationButtonColors()
    {
        ImGui::PopStyleColor(3);
    }

    void ContentBrowserPanel::SetNavigationBarColors()
    {
        if (ImGuiLayer::s_ImGuiTheme == Platform::Theme::ThemeType::Dark)
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.10f, 0.1f, 0.1f, .0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.2f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 0.5f));
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.3f, 0.3f, 0.3f, 0.2f));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 0.f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.1f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 0.5f));
        }
    }

    void ContentBrowserPanel::ResetNavigationBarColors()
    {
        ImGui::PopStyleColor(4);
    }

    void ContentBrowserPanel::SetAssetCardColors()
    {
        if (ImGuiLayer::s_ImGuiTheme == Platform::Theme::ThemeType::Dark)
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.10f, 0.1f, 0.1f, .0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 0.0f));
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.3f, 0.3f, 0.3f, 0.2f));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 0.f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 0.0f));
        }
    }

    void ContentBrowserPanel::ResetAssetCardColors()
    {
        ImGui::PopStyleColor(4);
    }

    void ContentBrowserPanel::DisplayNavigationBar()
    {
        float barHeight = ImGui::GetFrameHeight();

        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 4.0f);

        ImGui::BeginChild("NavigationBar", ImVec2(0.0f, barHeight), 0, ImGuiWindowFlags_NoScrollbar);
        {
            SetNavigationButtonColors();

            float buttonSize = ImGui::GetFrameHeight();
            ImGui::BeginDisabled(m_CurrentHistoryIndex == 0);
            if (ImGui::ImageButton("NavBack", m_LeftArrowIcon.get(), ImVec2(m_IconSize, m_IconSize)))
            {
                m_CurrentDirectory = m_History[--m_CurrentHistoryIndex];
            }
            ImGui::EndDisabled();

            ImGui::SameLine();

            ImGui::BeginDisabled(m_CurrentHistoryIndex == m_History.size() - 1);
            if (ImGui::ImageButton("NavForward", m_RightArrowIcon.get(), ImVec2(m_IconSize, m_IconSize)))
            {
                m_CurrentDirectory = m_History[++m_CurrentHistoryIndex];
            }
            ImGui::EndDisabled();

            ImGui::SameLine();

            ImGui::BeginDisabled(!m_CurrentDirectory.has_parent_path());
            if (ImGui::ImageButton("NavUp", m_TopArrowIcon.get(), ImVec2(m_IconSize, m_IconSize)))
            {
                if (m_CurrentDirectory.parent_path() != m_CurrentDirectory)
                {
                    m_CurrentDirectory = m_CurrentDirectory.parent_path();
                    m_History.push_back(m_CurrentDirectory);
                    m_CurrentHistoryIndex++;
                }
            }
            ImGui::EndDisabled();

            ResetNavigationButtonColors();

            ImGui::SameLine();

            uint32_t size = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;
            SetNavigationBarColors();
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.0f, 0.0f));
            ImGui::BeginChild("DirPath", ImVec2(0, size));
            {
                std::vector<std::string> dirTree{};
                std::filesystem::path currentPath{ m_CurrentDirectory };
                while (currentPath.parent_path() != currentPath)
                {
                    dirTree.emplace(dirTree.begin(), currentPath.filename().string());
                    currentPath = currentPath.parent_path();
                }
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
                if (ImGui::Button("/"))
                {
                    m_CurrentDirectory = std::filesystem::path("/");
                    m_History.push_back(m_CurrentDirectory);
                    m_CurrentHistoryIndex++;
                }
                ImGui::SameLine();
                for (uint32_t pathIndex = 0; pathIndex < dirTree.size(); pathIndex++)
                {
                    if (ImGui::Button(std::format("{}##DirPath{}", dirTree[pathIndex], pathIndex).c_str()))
                    {
                        std::filesystem::path newPath = "/";
                        for (uint32_t newPathIndex = 0; newPathIndex <= pathIndex; newPathIndex++)
                        {
                            newPath /= dirTree[newPathIndex];
                        }
                        m_CurrentDirectory = newPath;
                        m_History.push_back(m_CurrentDirectory);
                        m_CurrentHistoryIndex++;
                    }
                    ImGui::SameLine();

                    if (pathIndex < dirTree.size() - 1)
                    {
                        ImGui::TextDisabled("/");
                        ImGui::SameLine();
                    }
                }
                ImGui::PopStyleVar(2);
            }
            ImGui::EndChild();
            ResetNavigationBarColors();
        }
        ImGui::EndChild();

        ImGui::PopStyleVar();
    }

    void ContentBrowserPanel::DisplayAssetCard(const std::filesystem::directory_entry& dirEntry, float thumbnailSize, float padding)
    {
        float cellSize = thumbnailSize + padding;

        ImGui::PushID(dirEntry.path().string().c_str());

        ImGui::BeginGroup();

        Ref<Graphics::Texture2D> thumbnail = dirEntry.is_directory() ? m_FolderIcon : m_FileIcon;

        SetAssetCardColors();
        ImGui::ImageButton("##thumb", thumbnail.get(), ImVec2(thumbnailSize, thumbnailSize));
        ResetAssetCardColors();

        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + cellSize);
        ImVec2 textSize = ImGui::CalcTextSize(dirEntry.path().filename().string().c_str(), 0, false, cellSize);
        ImGui::SetCursorPosX( ImGui::GetCursorPosX() + (cellSize - textSize.x) * 0.5f );
        ImGui::TextUnformatted(dirEntry.path().filename().string().c_str());
        ImGui::PopTextWrapPos();

        ImGui::EndGroup();

        bool isSelected = dirEntry.path() == m_SelectedItem;
        bool isHovered = ImGui::IsItemHovered();

        if (isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            if (isSelected) m_SelectedItem.clear();
            else m_SelectedItem = dirEntry.path();
        }

        if (isSelected || isHovered)
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 minRect = ImGui::GetItemRectMin();
            ImVec2 maxRect = ImGui::GetItemRectMax();

            maxRect.y += 4.f;

            ImU32 highlightColor = isSelected ? ImGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.3f, 0.3f)) : ImGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.3f, 0.2f));

            drawList->AddRectFilled(minRect, maxRect, highlightColor, ImGui::GetStyle().FrameRounding);
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            if (dirEntry.is_directory())
            {
                if (m_CurrentHistoryIndex != m_History.size() - 1)
                {
                    m_History.erase(m_History.begin() + m_CurrentHistoryIndex + 1, m_History.end());
                }
                m_CurrentDirectory = dirEntry.path();
                m_History.push_back(m_CurrentDirectory);
                m_CurrentHistoryIndex++;
            }
        }

        ImGui::PopID();
    }
}
