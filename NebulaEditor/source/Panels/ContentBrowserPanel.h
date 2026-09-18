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

        void OnImGuiRender();

    private:
        void SetNavigationButtonColors();
        void ResetNavigationButtonColors();
        void SetNavigationBarColors();
        void ResetNavigationBarColors();
        void DisplayNavigationBar();

    private:
        std::filesystem::path m_CurrentDirectory;

        std::vector<std::filesystem::path> m_History{};
        uint32_t m_CurrentHistoryIndex = 0;

        uint32_t m_IconSize;
        Ref<Graphics::Device> m_Device;
        Ref<Graphics::Texture2D> m_LeftArrowIcon;
        Ref<Graphics::Texture2D> m_RightArrowIcon;
        Ref<Graphics::Texture2D> m_TopArrowIcon;
    };
}
