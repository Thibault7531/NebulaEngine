#include "Platform/Nebula/CoreSystems/Input/Input.h"

#include <Windows.h>

namespace Nebula::Platform
{

    bool Input::IsKeyPressed(KeyboardKey keycode)
    {
        BYTE state[256];
        if (!GetKeyboardState(state)) return false;
        return state[GetNativeKeyCode(keycode)] != 0;
    }

    bool Input::IsMouseButtonPressed(MouseButton button)
    {
        int nativeButton = -1;
        switch (button)
        {
        case MouseButton::Left: nativeButton = VK_LBUTTON; break;
        case MouseButton::Right: nativeButton = VK_RBUTTON; break;
        case MouseButton::Middle: nativeButton = VK_MBUTTON; break;
        case MouseButton::X1: nativeButton = VK_XBUTTON1; break;
        case MouseButton::X2: nativeButton = VK_XBUTTON2; break;
        default: break;
        }
        if (nativeButton == -1) return false;
        return HIWORD(GetKeyState(nativeButton)) == 1;
    }

    std::pair<float, float> Input::GetMousePosition()
    {
        POINT pos;
        GetCursorPos(&pos);
        return { pos.x, pos.y };
    }

    float Input::GetMouseX()
    {
        auto [x, y] = GetMousePosition();
        return x;
    }

    float Input::GetMouseY()
    {
        auto [x, y] = GetMousePosition();
        return y;
    }

    int32_t Input::GetNativeKeyCode(KeyboardKey keycode)
    {
        switch (keycode)
		{
			case KeyboardKey::Backspace: return VK_BACK;
			case KeyboardKey::Tab: return VK_TAB;
			case KeyboardKey::Clear: return VK_CLEAR;
			case KeyboardKey::Return: return VK_RETURN;
			case KeyboardKey::Pause: return VK_PAUSE;
			case KeyboardKey::CapsLock: return VK_CAPITAL;
			case KeyboardKey::Escape: return VK_ESCAPE;
			case KeyboardKey::Space: return VK_SPACE;
			case KeyboardKey::PageUp: return VK_PRIOR;
			case KeyboardKey::PageDown: return VK_NEXT;
			case KeyboardKey::End: return VK_END;
			case KeyboardKey::Home: return VK_HOME;
			case KeyboardKey::LeftArrow: return VK_LEFT;
			case KeyboardKey::UpArrow: return VK_UP;
			case KeyboardKey::RightArrow: return VK_RIGHT;
			case KeyboardKey::DownArrow: return VK_DOWN;
			case KeyboardKey::Select: return VK_SELECT;
			case KeyboardKey::ACPrint: return VK_PRINT;
			case KeyboardKey::Execute: return VK_EXECUTE;
			case KeyboardKey::PrintScreen: return VK_SNAPSHOT;
			case KeyboardKey::Insert: return VK_INSERT;
			case KeyboardKey::Delete: return VK_DELETE;
			case KeyboardKey::Help: return VK_HELP;
			case KeyboardKey::Num0: return '0';
			case KeyboardKey::Num1: return '1';
			case KeyboardKey::Num2: return '2';
			case KeyboardKey::Num3: return '3';
			case KeyboardKey::Num4: return '4';
			case KeyboardKey::Num5: return '5';
			case KeyboardKey::Num6: return '6';
			case KeyboardKey::Num7: return '7';
			case KeyboardKey::Num8: return '8';
			case KeyboardKey::Num9: return '9';
			case KeyboardKey::A: return 'A';
			case KeyboardKey::B: return 'B';
			case KeyboardKey::C: return 'C';
			case KeyboardKey::D: return 'D';
			case KeyboardKey::E: return 'E';
			case KeyboardKey::F: return 'F';
			case KeyboardKey::G: return 'G';
			case KeyboardKey::H: return 'H';
			case KeyboardKey::I: return 'I';
			case KeyboardKey::J: return 'J';
			case KeyboardKey::K: return 'K';
			case KeyboardKey::L: return 'L';
			case KeyboardKey::M: return 'M';
			case KeyboardKey::N: return 'N';
			case KeyboardKey::O: return 'O';
			case KeyboardKey::P: return 'P';
			case KeyboardKey::Q: return 'Q';
			case KeyboardKey::R: return 'R';
			case KeyboardKey::S: return 'S';
			case KeyboardKey::T: return 'T';
			case KeyboardKey::U: return 'U';
			case KeyboardKey::V: return 'V';
			case KeyboardKey::W: return 'W';
			case KeyboardKey::X: return 'X';
			case KeyboardKey::Y: return 'Y';
			case KeyboardKey::Z: return 'Z';
			case KeyboardKey::LeftGUI: return VK_LWIN;
			case KeyboardKey::RightGUI: return VK_RWIN;
			case KeyboardKey::Application: return VK_APPS;
			case KeyboardKey::Sleep: return VK_SLEEP;
			case KeyboardKey::Keypad0: return VK_NUMPAD0;
			case KeyboardKey::Keypad1: return VK_NUMPAD1;
			case KeyboardKey::Keypad2: return VK_NUMPAD2;
			case KeyboardKey::Keypad3: return VK_NUMPAD3;
			case KeyboardKey::Keypad4: return VK_NUMPAD4;
			case KeyboardKey::Keypad5: return VK_NUMPAD5;
			case KeyboardKey::Keypad6: return VK_NUMPAD6;
			case KeyboardKey::Keypad7: return VK_NUMPAD7;
			case KeyboardKey::Keypad8: return VK_NUMPAD8;
			case KeyboardKey::Keypad9: return VK_NUMPAD9;
			case KeyboardKey::KeypadMultiply: return VK_MULTIPLY;
			case KeyboardKey::KeypadPlus: return VK_ADD;
			case KeyboardKey::KeypadComma: return VK_SEPARATOR;
			case KeyboardKey::KeypadMinus: return VK_SUBTRACT;
			case KeyboardKey::KeypadDecimal: return VK_DECIMAL;
			case KeyboardKey::KeypadDivide: return VK_DIVIDE;
			case KeyboardKey::F1: return VK_F1;
			case KeyboardKey::F2: return VK_F2;
			case KeyboardKey::F3: return VK_F3;
			case KeyboardKey::F4: return VK_F4;
			case KeyboardKey::F5: return VK_F5;
			case KeyboardKey::F6: return VK_F6;
			case KeyboardKey::F7: return VK_F7;
			case KeyboardKey::F8: return VK_F8;
			case KeyboardKey::F9: return VK_F9;
			case KeyboardKey::F10: return VK_F10;
			case KeyboardKey::F11: return VK_F11;
			case KeyboardKey::F12: return VK_F12;
			case KeyboardKey::F13: return VK_F13;
			case KeyboardKey::F14: return VK_F14;
			case KeyboardKey::F15: return VK_F15;
			case KeyboardKey::F16: return VK_F16;
			case KeyboardKey::F17: return VK_F17;
			case KeyboardKey::F18: return VK_F18;
			case KeyboardKey::F19: return VK_F19;
			case KeyboardKey::F20: return VK_F20;
			case KeyboardKey::F21: return VK_F21;
			case KeyboardKey::F22: return VK_F22;
			case KeyboardKey::F23: return VK_F23;
			case KeyboardKey::F24: return VK_F24;
			case KeyboardKey::NumLock: return VK_NUMLOCK;
			case KeyboardKey::ScrollLock: return VK_SCROLL;
			case KeyboardKey::LeftShift: return VK_LSHIFT;
			case KeyboardKey::RightShift: return VK_RSHIFT;
			case KeyboardKey::LeftControl: return VK_LCONTROL;
			case KeyboardKey::RightControl: return VK_RCONTROL;
			case KeyboardKey::LeftAlt: return VK_LMENU;
			case KeyboardKey::RightAlt: return VK_RMENU;
			case KeyboardKey::ACBack: return VK_BROWSER_BACK;
			case KeyboardKey::ACForward: return VK_BROWSER_FORWARD;
			case KeyboardKey::ACRefresh: return VK_BROWSER_REFRESH;
			case KeyboardKey::ACStop: return VK_BROWSER_STOP;
			case KeyboardKey::ACSearch: return VK_BROWSER_SEARCH;
			case KeyboardKey::ACFavorites: return VK_BROWSER_FAVORITES;
			case KeyboardKey::ACHome: return VK_BROWSER_HOME;
			case KeyboardKey::Mute: return VK_VOLUME_MUTE;
			case KeyboardKey::VolumeDown: return VK_VOLUME_DOWN;
			case KeyboardKey::VolumeUp: return VK_VOLUME_UP;
			case KeyboardKey::AudioNextTrack: return VK_MEDIA_NEXT_TRACK;
			case KeyboardKey::AudioPreviousTrack: return VK_MEDIA_PREV_TRACK;
			case KeyboardKey::AudioStop: return VK_MEDIA_STOP;
			case KeyboardKey::AudioPlayPause: return VK_MEDIA_PLAY_PAUSE;
			case KeyboardKey::AudioSelect: return VK_LAUNCH_MEDIA_SELECT;
			default: break;
		}
		return -1;
    }
}
