#pragma once

#include <windows.h>
#include <winrt/Microsoft.UI.Windowing.h>

#include "Platform/Nebula/CoreSystems/Rendering/Context.h"
#include "Platform/Nebula/CoreSystems/Windows/Window.h"
#include "Platform/Platform/Windows/Events/EventHandler.h"

namespace Nebula::Platform
{

    class WindowsWindow : public Window
    {
    public:
        WindowsWindow(const WindowProperties& props);
        virtual ~WindowsWindow();

        virtual void SetEventCallback(const std::function<void(Event&)>& func) override;
        virtual const std::function<void(Event&)>& GetEventCallback() override;
        virtual void SetWindowHitTest(const std::function<HitTest(int32_t, int32_t)>& func) override;
        virtual void RecalculateHitTest() override;
        virtual const std::function<HitTest(int32_t, int32_t)>& GetWindowHitTest() override;

        virtual void OnUpdate() override;
        virtual void SwapBuffers() override;

        [[nodiscard]] virtual std::pair<unsigned int, unsigned int> GetSize() const override;
        virtual void SetSize(uint32_t width, uint32_t height) override;
        [[nodiscard]] virtual std::pair<uint32_t, uint32_t> GetClientSize() const override;
        virtual void SetClientSize(uint32_t width, uint32_t height) override;
        [[nodiscard]] virtual std::pair<int32_t, int32_t> GetPos() const override;
        virtual void SetPos(int32_t x, int32_t y) override;
        [[nodiscard]] virtual std::pair<int32_t, int32_t> GetClientPos() const override;
        [[nodiscard]] virtual int32_t GetTitlebarHeight() const override;

        virtual void SetMousePos(int32_t x, int32_t y) const override;
        virtual void CaptureMouse(bool enabled) override;

        virtual void SetVSync(VSyncMode mode) override;
        virtual VSyncMode IsVSync() const override;
        virtual void SetFullscreen(bool enabled) override;
        [[nodiscard]] virtual bool IsFullscreen() const override;
        virtual void SetMaximized(bool enabled) override;
        [[nodiscard]] virtual bool IsMaximized() const override;
        virtual void Minimize() override;
        [[nodiscard]] virtual bool IsMinimized() const override;
        [[nodiscard]] virtual bool IsFocused() const override;
        [[nodiscard]] virtual std::string GetTitle() const override;
        virtual void SetTitle(const std::string& title) override;

        virtual void Show() const override;
        virtual void Hide() const override;
        virtual void Raise() const override;
        virtual void Flash() const override;

        [[nodiscard]] virtual Ref<Context> GetContext() override { return m_Context; }
        virtual void SetCurrentContext() override;


    public:
        virtual void* GetNativeWindow() override { return &m_Window; }
        virtual void* GetHandle() const override;
        virtual uint32_t GetID() const override;

    private:
        std::vector<winrt::Windows::Graphics::RectInt32> FindDragRectsFromHitTest();

    private:
        winrt::Microsoft::UI::Windowing::AppWindow m_Window;
        HWND m_Hwnd;

        Ref<Context> m_Context;

        std::function<void(Event&)> m_EventCallback;
        std::function<HitTest(int32_t, int32_t)> m_WindowHitTest;
    };

}