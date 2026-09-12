#pragma once
#include <filesystem>

#include "Nebula/CoreSystems/Core/Core.h"
#include "Nebula/Scene/Scene.h"

namespace Nebula
{
    class ContentBrowserPanel
    {
    public:
        ContentBrowserPanel() = default;

        void OnImGuiRender();

    private:
        std::filesystem::path m_CurrentDirectory = "assets";
    };
}
