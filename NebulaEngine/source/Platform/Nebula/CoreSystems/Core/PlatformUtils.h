#pragma once

#include <cstring>
#include <string>
#include <vector>

#include "Platform/Nebula/CoreSystems/Windows/Window.h"

namespace Nebula::Platform
{

    class Displays
    {
    public:
        struct Display
        {
            int32_t x, y;
            uint32_t width, height;
            int32_t workX, workY;
            uint32_t workWidth, workHeight;
            bool primary;

            Display() { memset(this, 0, sizeof(*this)); }
        };

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
        enum class CursorType
        {
            Arrow, TextInput, Move, ResizeNS, ResizeEW, ResizeNESW, ResizeNWSE, Hand, NotAllowed
        };

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

}
