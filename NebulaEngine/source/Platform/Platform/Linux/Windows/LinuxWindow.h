#pragma once

#include <SDL3/SDL.h>

#include "Platform/Nebula/CoreSystems/Windows/Window.h"
#include "Platform/Platform/Linux/Events/EventHandler.h"

namespace Nebula::Platform
{

    class LinuxWindow : public Window
    {
    public:
        LinuxWindow(const WindowProperties& props);
        virtual ~LinuxWindow();

        virtual void SetEventCallback(const std::function<void(Event&)>& func) override { m_EventCallback = func; }
        virtual const std::function<void(Event&)>& GetEventCallback() override { return m_EventCallback; }
        virtual void SetWindowHitTest(const std::function<HitTest(int32_t, int32_t)>& func) override;
        virtual void RecalculateHitTest() override {}
        virtual const std::function<HitTest(int32_t, int32_t)>& GetWindowHitTest() override { return m_WindowHitTest; }

        virtual void OnUpdate() override;
        virtual void SwapBuffers() override;

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

        virtual Ref<Context> GetContext() override;
        virtual void SetCurrentContext() override;

    public:
        virtual void* GetNativeWindow() override { return m_Window; }
        [[nodiscard]] virtual void* GetHandle() override { return m_Window; }
        [[nodiscard]] virtual uint32_t GetID() const override { return SDL_GetWindowID(m_Window); }

    private:
        SDL_Window* m_Window;

        Ref<Context> m_Context;

        std::function<void(Event&)> m_EventCallback;
        std::function<HitTest(int32_t, int32_t)> m_WindowHitTest;
    };

}
