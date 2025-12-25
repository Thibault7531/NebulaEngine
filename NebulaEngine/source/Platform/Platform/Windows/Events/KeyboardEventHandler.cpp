#include "KeyboardEventHandler.h"

#include <winrt/Microsoft.UI.Windowing.h>

#include "Platform/Nebula/CoreSystems/Events/KeyEvent.h"

namespace Nebula::Platform
{

    KeyboardEventHandler::KeyboardEventHandler(Window* window)
        : m_Window{window}, m_InputKeyboardSource{nullptr}
    {
        winrt::Microsoft::UI::Windowing::AppWindow* NativeWindow = static_cast<winrt::Microsoft::UI::Windowing::AppWindow*>(m_Window->GetNativeWindow());

        m_InputKeyboardSource = winrt::Microsoft::UI::Input::InputKeyboardSource::GetForWindowId(NativeWindow->Id());
    }

    KeyboardEventHandler::~KeyboardEventHandler()
    {
    }

    void KeyboardEventHandler::InitEventHandlers()
    {
        m_InputKeyboardSource.KeyDown({ this, &KeyboardEventHandler::KeyDownHandler });
        m_InputKeyboardSource.KeyUp({ this, &KeyboardEventHandler::KeyUpHandler });
        m_InputKeyboardSource.CharacterReceived({ this, &KeyboardEventHandler::CharacterReceivedHandler });
    }

    void KeyboardEventHandler::KeyDownHandler(winrt::Microsoft::UI::Input::InputKeyboardSource keyboard,
        winrt::Microsoft::UI::Input::KeyEventArgs args)
    {
        winrt::Microsoft::UI::Windowing::AppWindow* NativeWindow = static_cast<winrt::Microsoft::UI::Windowing::AppWindow*>(m_Window->GetNativeWindow());

        args.Handled(true);

        KeyboardKey key = GetKeyFromVirtualKey(args.VirtualKey());
        KeyPressedEvent event = KeyPressedEvent(key, 0);
        event.WindowID = NativeWindow->Id().Value;
        m_EventCallback(event);
    }

    void KeyboardEventHandler::KeyUpHandler(winrt::Microsoft::UI::Input::InputKeyboardSource keyboard,
        winrt::Microsoft::UI::Input::KeyEventArgs args)
    {
        winrt::Microsoft::UI::Windowing::AppWindow* NativeWindow = static_cast<winrt::Microsoft::UI::Windowing::AppWindow*>(m_Window->GetNativeWindow());

        args.Handled(true);

        KeyboardKey key = GetKeyFromVirtualKey(args.VirtualKey());
        KeyReleasedEvent event = KeyReleasedEvent(key);
        event.WindowID = NativeWindow->Id().Value;
        m_EventCallback(event);
    }

    void KeyboardEventHandler::CharacterReceivedHandler(winrt::Microsoft::UI::Input::InputKeyboardSource keyboard,
        winrt::Microsoft::UI::Input::CharacterReceivedEventArgs args)
    {
        winrt::Microsoft::UI::Windowing::AppWindow* NativeWindow = static_cast<winrt::Microsoft::UI::Windowing::AppWindow*>(m_Window->GetNativeWindow());

        args.Handled(true);

        KeyTypedEvent event = KeyTypedEvent(static_cast<char>(args.KeyCode()));
        event.WindowID = NativeWindow->Id().Value;
        m_EventCallback(event);
    }

    KeyboardKey KeyboardEventHandler::GetKeyFromVirtualKey(winrt::Windows::System::VirtualKey virtualKey)
    {
        switch (virtualKey)
        {
        case winrt::Windows::System::VirtualKey::Cancel: return KeyboardKey::Cancel;
        case winrt::Windows::System::VirtualKey::Back: return KeyboardKey::ACBack;
        case winrt::Windows::System::VirtualKey::Tab: return KeyboardKey::Tab;
        case winrt::Windows::System::VirtualKey::Clear: return KeyboardKey::Clear;
        case winrt::Windows::System::VirtualKey::Enter: return KeyboardKey::Return;
        case winrt::Windows::System::VirtualKey::Shift: return KeyboardKey::LeftShift;
        case winrt::Windows::System::VirtualKey::Control: return KeyboardKey::LeftControl;
        case winrt::Windows::System::VirtualKey::Menu: return KeyboardKey::LeftGUI;
        case winrt::Windows::System::VirtualKey::Pause: return KeyboardKey::Pause;
        case winrt::Windows::System::VirtualKey::CapitalLock: return KeyboardKey::CapsLock;
        case winrt::Windows::System::VirtualKey::Escape: return KeyboardKey::Escape;
        case winrt::Windows::System::VirtualKey::Space: return KeyboardKey::Space;
        case winrt::Windows::System::VirtualKey::PageUp: return KeyboardKey::PageUp;
        case winrt::Windows::System::VirtualKey::PageDown: return KeyboardKey::PageDown;
        case winrt::Windows::System::VirtualKey::End: return KeyboardKey::End;
        case winrt::Windows::System::VirtualKey::Home: return KeyboardKey::Home;
        case winrt::Windows::System::VirtualKey::Left: return KeyboardKey::LeftArrow;
        case winrt::Windows::System::VirtualKey::Up: return KeyboardKey::UpArrow;
        case winrt::Windows::System::VirtualKey::Right: return KeyboardKey::RightArrow;
        case winrt::Windows::System::VirtualKey::Down: return KeyboardKey::DownArrow;
        case winrt::Windows::System::VirtualKey::Select: return KeyboardKey::Select;
        case winrt::Windows::System::VirtualKey::Print: return KeyboardKey::ACPrint;
        case winrt::Windows::System::VirtualKey::Execute: return KeyboardKey::Execute;
        case winrt::Windows::System::VirtualKey::Snapshot: return KeyboardKey::PrintScreen;
        case winrt::Windows::System::VirtualKey::Insert: return KeyboardKey::Insert;
        case winrt::Windows::System::VirtualKey::Delete: return KeyboardKey::Delete;
        case winrt::Windows::System::VirtualKey::Help: return KeyboardKey::Help;
        case winrt::Windows::System::VirtualKey::Number0: return KeyboardKey::Num0;
        case winrt::Windows::System::VirtualKey::Number1: return KeyboardKey::Num1;
        case winrt::Windows::System::VirtualKey::Number2: return KeyboardKey::Num2;
        case winrt::Windows::System::VirtualKey::Number3: return KeyboardKey::Num3;
        case winrt::Windows::System::VirtualKey::Number4: return KeyboardKey::Num4;
        case winrt::Windows::System::VirtualKey::Number5: return KeyboardKey::Num5;
        case winrt::Windows::System::VirtualKey::Number6: return KeyboardKey::Num6;
        case winrt::Windows::System::VirtualKey::Number7: return KeyboardKey::Num7;
        case winrt::Windows::System::VirtualKey::Number8: return KeyboardKey::Num8;
        case winrt::Windows::System::VirtualKey::Number9: return KeyboardKey::Num9;
        case winrt::Windows::System::VirtualKey::A: return KeyboardKey::A;
        case winrt::Windows::System::VirtualKey::B: return KeyboardKey::B;
        case winrt::Windows::System::VirtualKey::C: return KeyboardKey::C;
        case winrt::Windows::System::VirtualKey::D: return KeyboardKey::D;
        case winrt::Windows::System::VirtualKey::E: return KeyboardKey::E;
        case winrt::Windows::System::VirtualKey::F: return KeyboardKey::F;
        case winrt::Windows::System::VirtualKey::G: return KeyboardKey::G;
        case winrt::Windows::System::VirtualKey::H: return KeyboardKey::H;
        case winrt::Windows::System::VirtualKey::I: return KeyboardKey::I;
        case winrt::Windows::System::VirtualKey::J: return KeyboardKey::J;
        case winrt::Windows::System::VirtualKey::K: return KeyboardKey::K;
        case winrt::Windows::System::VirtualKey::L: return KeyboardKey::L;
        case winrt::Windows::System::VirtualKey::M: return KeyboardKey::M;
        case winrt::Windows::System::VirtualKey::N: return KeyboardKey::N;
        case winrt::Windows::System::VirtualKey::O: return KeyboardKey::O;
        case winrt::Windows::System::VirtualKey::P: return KeyboardKey::P;
        case winrt::Windows::System::VirtualKey::Q: return KeyboardKey::Q;
        case winrt::Windows::System::VirtualKey::R: return KeyboardKey::R;
        case winrt::Windows::System::VirtualKey::S: return KeyboardKey::S;
        case winrt::Windows::System::VirtualKey::T: return KeyboardKey::T;
        case winrt::Windows::System::VirtualKey::U: return KeyboardKey::U;
        case winrt::Windows::System::VirtualKey::V: return KeyboardKey::V;
        case winrt::Windows::System::VirtualKey::W: return KeyboardKey::W;
        case winrt::Windows::System::VirtualKey::X: return KeyboardKey::X;
        case winrt::Windows::System::VirtualKey::Y: return KeyboardKey::Y;
        case winrt::Windows::System::VirtualKey::Z: return KeyboardKey::Z;
        case winrt::Windows::System::VirtualKey::LeftWindows: return KeyboardKey::LeftGUI;
        case winrt::Windows::System::VirtualKey::RightWindows: return KeyboardKey::RightGUI;
        case winrt::Windows::System::VirtualKey::Application: return KeyboardKey::Application;
        case winrt::Windows::System::VirtualKey::Sleep: return KeyboardKey::Sleep;
        case winrt::Windows::System::VirtualKey::NumberPad0: return KeyboardKey::Keypad0;
        case winrt::Windows::System::VirtualKey::NumberPad1: return KeyboardKey::Keypad1;
        case winrt::Windows::System::VirtualKey::NumberPad2: return KeyboardKey::Keypad2;
        case winrt::Windows::System::VirtualKey::NumberPad3: return KeyboardKey::Keypad3;
        case winrt::Windows::System::VirtualKey::NumberPad4: return KeyboardKey::Keypad4;
        case winrt::Windows::System::VirtualKey::NumberPad5: return KeyboardKey::Keypad5;
        case winrt::Windows::System::VirtualKey::NumberPad6: return KeyboardKey::Keypad6;
        case winrt::Windows::System::VirtualKey::NumberPad7: return KeyboardKey::Keypad7;
        case winrt::Windows::System::VirtualKey::NumberPad8: return KeyboardKey::Keypad8;
        case winrt::Windows::System::VirtualKey::NumberPad9: return KeyboardKey::Keypad9;
        case winrt::Windows::System::VirtualKey::Multiply: return KeyboardKey::KeypadMultiply;
        case winrt::Windows::System::VirtualKey::Add: return KeyboardKey::KeypadPlus;
        case winrt::Windows::System::VirtualKey::Separator: return KeyboardKey::KeypadComma;
        case winrt::Windows::System::VirtualKey::Subtract: return KeyboardKey::KeypadMinus;
        case winrt::Windows::System::VirtualKey::Decimal: return KeyboardKey::KeypadDecimal;
        case winrt::Windows::System::VirtualKey::Divide: return KeyboardKey::KeypadDivide;
        case winrt::Windows::System::VirtualKey::F1: return KeyboardKey::F1;
        case winrt::Windows::System::VirtualKey::F2: return KeyboardKey::F2;
        case winrt::Windows::System::VirtualKey::F3: return KeyboardKey::F3;
        case winrt::Windows::System::VirtualKey::F4: return KeyboardKey::F4;
        case winrt::Windows::System::VirtualKey::F5: return KeyboardKey::F5;
        case winrt::Windows::System::VirtualKey::F6: return KeyboardKey::F6;
        case winrt::Windows::System::VirtualKey::F7: return KeyboardKey::F7;
        case winrt::Windows::System::VirtualKey::F8: return KeyboardKey::F8;
        case winrt::Windows::System::VirtualKey::F9: return KeyboardKey::F9;
        case winrt::Windows::System::VirtualKey::F10: return KeyboardKey::F10;
        case winrt::Windows::System::VirtualKey::F11: return KeyboardKey::F11;
        case winrt::Windows::System::VirtualKey::F12: return KeyboardKey::F12;
        case winrt::Windows::System::VirtualKey::F13: return KeyboardKey::F13;
        case winrt::Windows::System::VirtualKey::F14: return KeyboardKey::F14;
        case winrt::Windows::System::VirtualKey::F15: return KeyboardKey::F15;
        case winrt::Windows::System::VirtualKey::F16: return KeyboardKey::F16;
        case winrt::Windows::System::VirtualKey::F17: return KeyboardKey::F17;
        case winrt::Windows::System::VirtualKey::F18: return KeyboardKey::F18;
        case winrt::Windows::System::VirtualKey::F19: return KeyboardKey::F19;
        case winrt::Windows::System::VirtualKey::F20: return KeyboardKey::F20;
        case winrt::Windows::System::VirtualKey::F21: return KeyboardKey::F21;
        case winrt::Windows::System::VirtualKey::F22: return KeyboardKey::F22;
        case winrt::Windows::System::VirtualKey::F23: return KeyboardKey::F23;
        case winrt::Windows::System::VirtualKey::F24: return KeyboardKey::F24;
        case winrt::Windows::System::VirtualKey::Scroll: return KeyboardKey::ScrollLock;
        case winrt::Windows::System::VirtualKey::LeftShift: return KeyboardKey::LeftShift;
        case winrt::Windows::System::VirtualKey::RightShift: return KeyboardKey::RightShift;
        case winrt::Windows::System::VirtualKey::LeftControl: return KeyboardKey::LeftControl;
        case winrt::Windows::System::VirtualKey::RightControl: return KeyboardKey::RightControl;
        case winrt::Windows::System::VirtualKey::LeftMenu: return KeyboardKey::LeftAlt;
        case winrt::Windows::System::VirtualKey::RightMenu: return KeyboardKey::RightAlt;
        case winrt::Windows::System::VirtualKey::GoBack: return KeyboardKey::ACBack;
        case winrt::Windows::System::VirtualKey::GoForward: return KeyboardKey::ACForward;
        case winrt::Windows::System::VirtualKey::Refresh: return KeyboardKey::ACRefresh;
        case winrt::Windows::System::VirtualKey::Stop: return KeyboardKey::ACStop;
        case winrt::Windows::System::VirtualKey::Search: return KeyboardKey::ACSearch;
        case winrt::Windows::System::VirtualKey::Favorites: return KeyboardKey::ACFavorites;
        case winrt::Windows::System::VirtualKey::GoHome: return KeyboardKey::ACHome;
        default: return KeyboardKey::Unknown;
        }
    }
}
