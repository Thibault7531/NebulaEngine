#pragma once
#include <filesystem>

#include "Nebula/CoreSystems/Core/Core.h"
#include "Nebula/Scene/Scene.h"

namespace Nebula
{
    class ContentBrowserPanel
    {
    public:
        ContentBrowserPanel();

        void OnImGuiRender();

    private:
        void DisplayNavigationBar();

    private:
        std::filesystem::path m_CurrentDirectory;

        std::vector<std::filesystem::path> m_History{};
        uint32_t m_CurrentHistoryIndex = 0;
    };
}
