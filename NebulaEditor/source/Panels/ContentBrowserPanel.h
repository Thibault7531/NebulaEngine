#pragma once
#include <filesystem>

#include "Nebula/CoreSystems/Core/Core.h"
#include "Nebula/Graphics/Device.h"
#include "Nebula/Graphics/Texture.h"
#include "Nebula/Scene/Scene.h"

namespace Nebula
{
    class ContentBrowserPanel
    {
    public:
        ContentBrowserPanel();

        void SetOpenSceneCallback(std::function<void(std::filesystem::path)> callback) { m_OpenSceneCallback = callback; }

        void OnImGuiRender();

    private:
        void SetNavigationButtonColors();
        void ResetNavigationButtonColors();
        void SetNavigationBarColors();
        void ResetNavigationBarColors();
        void SetAssetCardColors();
        void ResetAssetCardColors();
        void DisplayNavigationBar();
        void DisplayAssetCard(const std::filesystem::directory_entry& dirEntry, float thumbnailSize, float padding);
        void SetDragDropSource(const std::filesystem::path& path, Ref<Graphics::Texture2D> thumbnail);

    private:
        Ref<Graphics::Texture2D> GetIconForAsset(const std::filesystem::path& path);

        bool IsSceneFile(const std::filesystem::path& path) { return path.extension() == ".nbscene"; }
        bool IsTextureFile(const std::filesystem::path& path);

    private:
        std::filesystem::path m_CurrentDirectory;
        std::filesystem::path m_SelectedItem;

        std::vector<std::filesystem::path> m_History{};
        uint32_t m_CurrentHistoryIndex = 0;

        uint32_t m_IconSize;
        uint32_t m_ThumbnailSize;
        Ref<Graphics::Device> m_Device;
        Ref<Graphics::Texture2D> m_LeftArrowIcon;
        Ref<Graphics::Texture2D> m_RightArrowIcon;
        Ref<Graphics::Texture2D> m_TopArrowIcon;
        Ref<Graphics::Texture2D> m_FolderIcon;
        Ref<Graphics::Texture2D> m_FileIcon;
        Ref<Graphics::Texture2D> m_SceneIcon;
        Ref<Graphics::Texture2D> m_TextureIcon;

        std::function<void(std::filesystem::path)> m_OpenSceneCallback;
    };
}
