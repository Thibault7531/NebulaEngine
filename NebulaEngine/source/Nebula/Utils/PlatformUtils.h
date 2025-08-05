#pragma once
#include "Nebula/CoreSystems/Input/KeyCodes.h"

namespace Nebula
{

    class FileDialogs
    {
    public:
        enum class FileType
        {
            NebulaScene
        };

        static std::string OpenFile(const std::vector<FileType>& fileTypes);
        static std::string SaveFile(const char* filter);

    private:
        static void ConvertFilter(const std::vector<FileType>& fileTypes, char* filter);
    };

    class Displays
    {
    public:
        struct display
        {
            int x, y;
            int width, height;
            int workX, workY;
            int workWidth, workHeight;
            float dpi;
            bool primary;

            void* nativeDisplay;

            display() { memset(this, 0, sizeof(*this)); }
        };

    public:
        static std::vector<display> GetDisplays();
    };

    class WindowUtils
    {
    public:
        static Ref<Window> GetForegroundWindow();
    };

    class Mouse
    {
    public:
        static void ShowCursor();
        static void HideCursor();

        static void CaptureMouse(bool enable);

        static void SetMousePosGlobal(int x, int y);
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

        void Show() const;

    private:
        void* m_Cursor;
    };

    class Keyboard
    {
    public:
        static Ref<Window> GetCapturingWindow();

        static int GetScancodeFromKeycode(KeyboardKey keycode);
    };

    class Clipboard
    {
    public:
        static void SetText(const std::string& text);
        static std::string GetText();
    };

}
