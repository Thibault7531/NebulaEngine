#pragma once

#include "Nebula/CoreSystems/Input/KeyCodes.h"
#include "Nebula/CoreSystems/Windows/Window.h"

namespace Nebula
{

    class WindowsWindow : public Window
    {
    public:
        WindowsWindow(const WindowProps& props);
        virtual ~WindowsWindow() override;

        virtual void OnUpdate() override;
        virtual void Shutdown() override;

        virtual void SwapBuffers() const override;

        virtual void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
        virtual void SetWindowHitTest(std::function<HitTest(int, int)> callback) override;

        [[nodiscard]] virtual std::pair<unsigned int, unsigned int> GetSize() const override { return { m_Data.Width, m_Data.Height }; }
        virtual void SetSize(int w, int h) override;
        [[nodiscard]] virtual std::pair<unsigned int, unsigned int> GetPixelSize() const override;
        [[nodiscard]] virtual std::pair<int, int> GetPos() const override;
        virtual void SetPos(int x, int y) override;

        virtual void SetMousePos(int x, int y) const override;

        virtual void SetVSync(bool enabled) override;
        [[nodiscard]] virtual bool IsVSync() const override { return m_Data.VSync; }
        virtual void SetFullscreen(bool enabled) override;
        [[nodiscard]] virtual bool IsFullscreen() const override { return m_Data.Fullscreen; }
        virtual void SetMaximized(bool enabled) override;
        [[nodiscard]] virtual bool IsMaximized() const override { return m_Data.Maximized; }
        virtual void Minimize() override;
        [[nodiscard]] virtual bool IsMinimized() const override { return m_Data.Minimized; }
        [[nodiscard]] virtual bool IsHighDPI() const override { return m_Data.HighDPI; }
        [[nodiscard]] virtual bool IsFocused() const override;
        [[nodiscard]] virtual std::string GetTitle() const override { return m_Data.Title; }
        virtual void SetTitle(const std::string& title) override;
        virtual void SetAlpha(float alpha) override;
        virtual void SetParentWindow(Ref<Window> parent) const override;

        virtual void Show() const override;
        virtual void Hide() const override;
        virtual void Raise() const override;
        virtual void Flash() const override;

        [[nodiscard]] virtual Ref<GraphicsContext> GetContext() const override { return m_Context; }
        virtual void SetCurrentContext() override;

        [[nodiscard]] virtual inline void* GetNativeWindow() const override { return m_Window; }
        [[nodiscard]] virtual inline void* GetHandle() const override { return m_Window; }
        [[nodiscard]] virtual inline uint32_t GetID() const override { return m_WindowID; }
        virtual void SetID(int ID) override;

    private:
        static LRESULT OnEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        static KeyboardKey GetKeycode(DWORD keycode);

        static LRESULT PASCAL ErrorCallback(HWND hwnd, int nErrID, LPTSTR lpErrorText);

        void RegisterWindowClass();

    private:
        struct WindowData
        {
            std::string Title;
            int x, y;
            unsigned int Width, Height;
            bool Border;
            bool VSync;
            bool Maximized;
            bool Minimized = false;
            bool Fullscreen;
            bool HighDPI;
            int MouseTrackedArea = 2;
            RendererAPI::API API;

            EventCallbackFn EventCallback;
            std::function<HitTest(int, int)> HitTestCallback = nullptr;
        };

        WindowData m_Data;

    private:
        uint32_t m_WindowID;

        static ATOM s_WindowClassAtom;
        static WNDCLASSEXA s_WindowClass;
        static HINSTANCE s_hInstance;
        HWND m_Window;

        DWORD m_Style;
        DWORD m_ExStyle;

        Ref<GraphicsContext> m_Context;

    private:
        static bool s_ClassRegistered;
    };

}
