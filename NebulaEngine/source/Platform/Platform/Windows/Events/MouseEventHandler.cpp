#include "MouseEventHandler.h"

#include <winrt/Microsoft.UI.Windowing.h>

#include "Platform/Nebula/CoreSystems/Core/Log.h"
#include "Platform/Nebula/CoreSystems/Events/MouseEvent.h"

namespace Nebula::Platform
{
    MouseEventHandler::MouseEventHandler(Window* window)
        : m_Window{window}, m_InputPointerSource{nullptr}, m_InputNonClientPointerSource{nullptr}
    {
        winrt::Microsoft::UI::Windowing::AppWindow* NativeWindow = static_cast<winrt::Microsoft::UI::Windowing::AppWindow*>(m_Window->GetNativeWindow());

        m_InputPointerSource = winrt::Microsoft::UI::Input::InputPointerSource::GetForWindowId(NativeWindow->Id());
        if (m_Window->GetProperties().Style == WindowStyle::Default || m_Window->GetProperties().Style == WindowStyle::BorderlessInteractive) m_InputNonClientPointerSource = winrt::Microsoft::UI::Input::InputNonClientPointerSource::GetForWindowId(NativeWindow->Id());
    }

    MouseEventHandler::~MouseEventHandler()
    {
    }

    void MouseEventHandler::InitEventHandlers()
    {
        m_InputPointerSource.PointerMoved({ this, &MouseEventHandler::PointerMovedHandler });
        m_InputPointerSource.PointerWheelChanged({ this, &MouseEventHandler::PointerWheelChangedHandler });
        m_InputPointerSource.PointerPressed({ this, &MouseEventHandler::PointerPressedHandler });
        m_InputPointerSource.PointerReleased({ this, &MouseEventHandler::PointerReleasedHandler });
        m_InputPointerSource.PointerEntered({ this, &MouseEventHandler::PointerEnteredHandler });
        m_InputPointerSource.PointerExited({ this, &MouseEventHandler::PointerExitedHandler });

        if (m_Window->GetProperties().Style == WindowStyle::Default || m_Window->GetProperties().Style == WindowStyle::BorderlessInteractive)
        {
            m_InputNonClientPointerSource.PointerMoved({ this, &MouseEventHandler::NonClientPointerMovedHandler });
            m_InputNonClientPointerSource.PointerPressed({ this, &MouseEventHandler::NonClientPointerPressedHandler });
            m_InputNonClientPointerSource.PointerReleased({ this, &MouseEventHandler::NonClientPointerReleasedHandler });
        }
    }

    void MouseEventHandler::PointerMovedHandler(winrt::Microsoft::UI::Input::InputPointerSource pointer,
        winrt::Microsoft::UI::Input::PointerEventArgs args)
    {
        winrt::Microsoft::UI::Windowing::AppWindow* NativeWindow = static_cast<winrt::Microsoft::UI::Windowing::AppWindow*>(m_Window->GetNativeWindow());

        args.Handled(true);

        MouseMovedEvent event = MouseMovedEvent(args.CurrentPoint().Position().X, args.CurrentPoint().Position().Y);
        event.WindowID = NativeWindow->Id().Value;
        m_EventCallback(event);
    }

    void MouseEventHandler::PointerWheelChangedHandler(winrt::Microsoft::UI::Input::InputPointerSource pointer,
        winrt::Microsoft::UI::Input::PointerEventArgs args)
    {
        winrt::Microsoft::UI::Windowing::AppWindow* NativeWindow = static_cast<winrt::Microsoft::UI::Windowing::AppWindow*>(m_Window->GetNativeWindow());

        args.Handled(true);

        if (args.CurrentPoint().Properties().IsHorizontalMouseWheel())
        {
            MouseScrolledEvent event = MouseScrolledEvent(0.f, args.CurrentPoint().Properties().MouseWheelDelta());
            event.WindowID = NativeWindow->Id().Value;
            m_EventCallback(event);
        }
        else
        {
            MouseScrolledEvent event = MouseScrolledEvent(args.CurrentPoint().Properties().MouseWheelDelta(), 0.f);
            event.WindowID = NativeWindow->Id().Value;
            m_EventCallback(event);
        }
    }

    void MouseEventHandler::PointerPressedHandler(winrt::Microsoft::UI::Input::InputPointerSource pointer,
        winrt::Microsoft::UI::Input::PointerEventArgs args)
    {
        winrt::Microsoft::UI::Windowing::AppWindow* NativeWindow = static_cast<winrt::Microsoft::UI::Windowing::AppWindow*>(m_Window->GetNativeWindow());

        args.Handled(true);

        if (args.CurrentPoint().Properties().IsLeftButtonPressed() && !m_CurrentState.Left)
        {
            MouseButtonPressedEvent event = MouseButtonPressedEvent(MouseButton::Left);
            event.WindowID = NativeWindow->Id().Value;
            m_EventCallback(event);
            m_CurrentState.Left = true;
        }
        if (args.CurrentPoint().Properties().IsRightButtonPressed() && !m_CurrentState.Right)
        {
            MouseButtonPressedEvent event = MouseButtonPressedEvent(MouseButton::Right);
            event.WindowID = NativeWindow->Id().Value;
            m_EventCallback(event);
            m_CurrentState.Right = true;
        }
        if (args.CurrentPoint().Properties().IsMiddleButtonPressed() && !m_CurrentState.Middle)
        {
            MouseButtonPressedEvent event = MouseButtonPressedEvent(MouseButton::Middle);
            event.WindowID = NativeWindow->Id().Value;
            m_EventCallback(event);
            m_CurrentState.Middle = true;
        }
        if (args.CurrentPoint().Properties().IsXButton1Pressed() && !m_CurrentState.X1)
        {
            MouseButtonPressedEvent event = MouseButtonPressedEvent(MouseButton::X1);
            event.WindowID = NativeWindow->Id().Value;
            m_EventCallback(event);
            m_CurrentState.X1 = true;
        }
        if (args.CurrentPoint().Properties().IsXButton2Pressed() && !m_CurrentState.X2)
        {
            MouseButtonPressedEvent event = MouseButtonPressedEvent(MouseButton::X2);
            event.WindowID = NativeWindow->Id().Value;
            m_EventCallback(event);
            m_CurrentState.X2 = true;
        }
    }

    void MouseEventHandler::PointerReleasedHandler(winrt::Microsoft::UI::Input::InputPointerSource pointer,
        winrt::Microsoft::UI::Input::PointerEventArgs args)
    {
        winrt::Microsoft::UI::Windowing::AppWindow* NativeWindow = static_cast<winrt::Microsoft::UI::Windowing::AppWindow*>(m_Window->GetNativeWindow());

        args.Handled(true);

        if (!args.CurrentPoint().Properties().IsLeftButtonPressed() && m_CurrentState.Left)
        {
            MouseButtonReleasedEvent event = MouseButtonReleasedEvent(MouseButton::Left);
            event.WindowID = NativeWindow->Id().Value;
            m_EventCallback(event);
            m_CurrentState.Left = false;
        }
        if (!args.CurrentPoint().Properties().IsRightButtonPressed() && m_CurrentState.Right)
        {
            MouseButtonReleasedEvent event = MouseButtonReleasedEvent(MouseButton::Right);
            event.WindowID = NativeWindow->Id().Value;
            m_EventCallback(event);
            m_CurrentState.Right = false;
        }
        if (!args.CurrentPoint().Properties().IsMiddleButtonPressed() && m_CurrentState.Middle)
        {
            MouseButtonReleasedEvent event = MouseButtonReleasedEvent(MouseButton::Middle);
            event.WindowID = NativeWindow->Id().Value;
            m_EventCallback(event);
            m_CurrentState.Middle = false;
        }
        if (!args.CurrentPoint().Properties().IsXButton1Pressed() && m_CurrentState.X1)
        {
            MouseButtonReleasedEvent event = MouseButtonReleasedEvent(MouseButton::X1);
            event.WindowID = NativeWindow->Id().Value;
            m_EventCallback(event);
            m_CurrentState.X1 = false;
        }
        if (!args.CurrentPoint().Properties().IsXButton2Pressed() && m_CurrentState.X2)
        {
            MouseButtonReleasedEvent event = MouseButtonReleasedEvent(MouseButton::X2);
            event.WindowID = NativeWindow->Id().Value;
            m_EventCallback(event);
            m_CurrentState.X2 = false;
        }
    }

    void MouseEventHandler::PointerEnteredHandler(winrt::Microsoft::UI::Input::InputPointerSource pointer,
        winrt::Microsoft::UI::Input::PointerEventArgs args)
    {
        winrt::Microsoft::UI::Windowing::AppWindow* NativeWindow = static_cast<winrt::Microsoft::UI::Windowing::AppWindow*>(m_Window->GetNativeWindow());

        args.Handled(true);

        MouseEnteredEvent event = MouseEnteredEvent();
        event.WindowID = NativeWindow->Id().Value;
        m_EventCallback(event);
    }

    void MouseEventHandler::PointerExitedHandler(winrt::Microsoft::UI::Input::InputPointerSource pointer,
        winrt::Microsoft::UI::Input::PointerEventArgs args)
    {
        winrt::Microsoft::UI::Windowing::AppWindow* NativeWindow = static_cast<winrt::Microsoft::UI::Windowing::AppWindow*>(m_Window->GetNativeWindow());

        args.Handled(true);

        MouseLeftEvent event = MouseLeftEvent();
        event.WindowID = NativeWindow->Id().Value;
        m_EventCallback(event);
    }

    void MouseEventHandler::NonClientPointerMovedHandler(
        winrt::Microsoft::UI::Input::InputNonClientPointerSource pointer,
        winrt::Microsoft::UI::Input::NonClientPointerEventArgs args)
    {
        winrt::Microsoft::UI::Windowing::AppWindow* NativeWindow = static_cast<winrt::Microsoft::UI::Windowing::AppWindow*>(m_Window->GetNativeWindow());

        if (m_Window->GetProperties().Style == WindowStyle::BorderlessInteractive)
        {
            MouseMovedEvent event = MouseMovedEvent(args.Point().X, args.Point().Y);
            event.WindowID = NativeWindow->Id().Value;
            m_EventCallback(event);
        }
    }

    void MouseEventHandler::NonClientPointerPressedHandler(
        winrt::Microsoft::UI::Input::InputNonClientPointerSource pointer,
        winrt::Microsoft::UI::Input::NonClientPointerEventArgs args)
    {
        winrt::Microsoft::UI::Windowing::AppWindow* NativeWindow = static_cast<winrt::Microsoft::UI::Windowing::AppWindow*>(m_Window->GetNativeWindow());

        if (m_Window->GetProperties().Style == WindowStyle::BorderlessInteractive)
        {
            MouseButtonPressedEvent event = MouseButtonPressedEvent(MouseButton::Left);
            event.WindowID = NativeWindow->Id().Value;
            m_EventCallback(event);
        }
    }

    void MouseEventHandler::NonClientPointerReleasedHandler(
        winrt::Microsoft::UI::Input::InputNonClientPointerSource pointer,
        winrt::Microsoft::UI::Input::NonClientPointerEventArgs args)
    {
        winrt::Microsoft::UI::Windowing::AppWindow* NativeWindow = static_cast<winrt::Microsoft::UI::Windowing::AppWindow*>(m_Window->GetNativeWindow());

        if (m_Window->GetProperties().Style == WindowStyle::BorderlessInteractive)
        {
            MouseButtonReleasedEvent event = MouseButtonReleasedEvent(MouseButton::Left);
            event.WindowID = NativeWindow->Id().Value;
            m_EventCallback(event);
        }
    }
}
