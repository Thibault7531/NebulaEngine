#pragma once

#include "Platform/Nebula/CoreSystems/Windows/Window.h"
#include "Platform/Platform/MacOS/Events/EventHandler.h"
#include "Platform/Platform/MacOS/Rendering/OpenGLContext.h"

#include <SwiftMacOSWindow.h>

namespace Nebula::Platform
{

    class MacOSWindow : public Window
    {
    public:
        MacOSWindow(WindowProperties props);
        virtual ~MacOSWindow();

        virtual void SetEventCallback(const std::function<void(Event&)>& func) override;
        virtual const std::function<void(Event&)>& GetEventCallback() override;
        virtual void SetWindowHitTest(const std::function<HitTest(int32_t, int32_t)>& func) override;
        virtual void RecalculateHitTest() override;
        virtual const std::function<HitTest(int32_t, int32_t)>& GetWindowHitTest() override;

        virtual void OnUpdate() override;
        virtual void SwapBuffers() override;
        virtual void Present(uint32_t framebuffer) override;

        [[nodiscard]] virtual std::pair<uint32_t, uint32_t> GetSize() const override;
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
        [[nodiscard]] virtual VSyncMode IsVSync() const override;
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

        virtual void StartInput() override {}
        virtual void StopInput() override {}

        [[nodiscard]] virtual Ref<Context> GetContext() override;
        virtual void SetCurrentContext() override;

    public:
        virtual void* GetNativeWindow() override;
        virtual void* GetHandle() override;
        virtual uint32_t GetID() const override;

    private:
        EventHandler m_EventHandler;
        SwiftMacOSWindow::MacOSWindow m_NativeWindow;
        Ref<Context> m_Context;

        std::function<void(Event&)> m_EventCallback;
        std::function<HitTest(int32_t, int32_t)> m_WindowHitTest;
    };

}