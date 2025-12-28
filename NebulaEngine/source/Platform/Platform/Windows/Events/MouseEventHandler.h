#pragma once

#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Windows.Foundation.h>

#include "Platform/Nebula/CoreSystems/Windows/Window.h"

namespace Nebula::Platform
{

    struct MouseButtonState
    {
        bool Left = false;
        bool Right = false;
        bool Middle = false;
        bool X1 = false;
        bool X2 = false;
    };

    class MouseEventHandler
    {
    public:
        MouseEventHandler(Window* window);
        ~MouseEventHandler();

        void InitEventHandlers();

        void SetEventCallback(const std::function<void(Event&)>& callback) { m_EventCallback = callback; }
        void SetWindowHitTest(const std::function<HitTest(int32_t, int32_t)>& func) { m_WindowHitTest = func; }

    public:
        void PointerMovedHandler(winrt::Microsoft::UI::Input::InputPointerSource pointer, winrt::Microsoft::UI::Input::PointerEventArgs args);

        void PointerWheelChangedHandler(winrt::Microsoft::UI::Input::InputPointerSource pointer, winrt::Microsoft::UI::Input::PointerEventArgs args);

        void PointerPressedHandler(winrt::Microsoft::UI::Input::InputPointerSource pointer, winrt::Microsoft::UI::Input::PointerEventArgs args);
        void PointerReleasedHandler(winrt::Microsoft::UI::Input::InputPointerSource pointer, winrt::Microsoft::UI::Input::PointerEventArgs args);

        void PointerEnteredHandler(winrt::Microsoft::UI::Input::InputPointerSource pointer, winrt::Microsoft::UI::Input::PointerEventArgs args);
        void PointerExitedHandler(winrt::Microsoft::UI::Input::InputPointerSource pointer, winrt::Microsoft::UI::Input::PointerEventArgs args);

        void NonClientPointerMovedHandler(winrt::Microsoft::UI::Input::InputNonClientPointerSource pointer, winrt::Microsoft::UI::Input::NonClientPointerEventArgs args);
        void NonClientPointerPressedHandler(winrt::Microsoft::UI::Input::InputNonClientPointerSource pointer, winrt::Microsoft::UI::Input::NonClientPointerEventArgs args);
        void NonClientPointerReleasedHandler(winrt::Microsoft::UI::Input::InputNonClientPointerSource pointer, winrt::Microsoft::UI::Input::NonClientPointerEventArgs args);

        void HitTestHandler(winrt::Microsoft::UI::Input::InputPointerSource pointer, winrt::Microsoft::UI::Input::PointerEventArgs args);

    private:
        Window* m_Window;
        MouseButtonState m_CurrentState;

        std::function<void(Event&)> m_EventCallback;
        std::function<HitTest(int32_t, int32_t)> m_WindowHitTest;

        winrt::Microsoft::UI::Input::InputPointerSource m_InputPointerSource;
        winrt::Microsoft::UI::Input::InputNonClientPointerSource m_InputNonClientPointerSource;
    };

}
