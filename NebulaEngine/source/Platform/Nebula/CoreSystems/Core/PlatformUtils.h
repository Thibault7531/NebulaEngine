#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "Display.h"
#include "../Windows/Window.h"
#include "Platform/Nebula/CoreSystems/Input/CursorTypes.h"

namespace Nebula::Platform
{

    class Displays
    {
    public:
        static std::vector<Display> GetDisplays();
    };

    class Mouse
    {
    public:
        static void ShowCursor();
        static void HideCursor();

        static void SetMousePosGlobal(int32_t x, int32_t y);
        static std::pair<float, float> GetMousePos();
    };

    class Cursor
    {
    public:
        Cursor(CursorType type);
        ~Cursor() = default;

        void Show() const;

    private:
        void* m_Cursor;
    };

    class Clipboard
    {
    public:
        static void SetText(const std::string& text);
        static std::string GetText();
    };

    class WindowUtils
    {
    public:
        static Ref<Window> GetForegroundWindow();
    };

    class Files
    {
    public:
        static std::filesystem::path GetResourcesPath();
        static std::filesystem::path GetUserDataPath();

        static void setAppName(const std::string& appName) { s_AppName = appName; }

    private:
        static std::string s_AppName;
    };

}
