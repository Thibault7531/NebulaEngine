#include "Platform/Nebula/CoreSystems/Input/Input.h"

#include <SDL3/SDL.h>

namespace Nebula::Platform
{

    bool Input::IsKeyPressed(KeyboardKey keycode)
    {
        SDL_Keycode nativeKeycode = GetNativeKeyCode(keycode);
        return SDL_GetKeyboardState(nullptr)[SDL_GetScancodeFromKey(nativeKeycode, nullptr)] != 0;
    }

    bool Input::IsMouseButtonPressed(MouseButton button)
    {
        uint32_t state = SDL_GetMouseState(nullptr, nullptr);
        int32_t nativeButton = -1;
        switch (button)
        {
            case MouseButton::Left: nativeButton = SDL_BUTTON_LEFT; break;
            case MouseButton::Right: nativeButton = SDL_BUTTON_RIGHT; break;
            case MouseButton::Middle: nativeButton = SDL_BUTTON_MIDDLE; break;
            case MouseButton::X1: nativeButton = SDL_BUTTON_X1; break;
            case MouseButton::X2: nativeButton = SDL_BUTTON_X2; break;
            default: break;
        }
        if (nativeButton == -1) return false;
        return (state & SDL_BUTTON_MASK(nativeButton)) != 0;
    }

    std::pair<float, float> Input::GetMousePosition()
    {
        float x, y;
        SDL_GetGlobalMouseState(&x, &y);
        return {x, y};
    }

    float Input::GetMouseX()
    {
        return GetMousePosition().first;
    }

    float Input::GetMouseY()
    {
        return GetMousePosition().second;
    }

    int32_t Input::GetNativeKeyCode(KeyboardKey keycode)
    {
        switch (keycode)
        {
        case KeyboardKey::Unknown: return SDLK_UNKNOWN;
        case KeyboardKey::Return: return SDLK_RETURN;
        case KeyboardKey::Escape: return SDLK_ESCAPE;
        case KeyboardKey::Backspace: return SDLK_BACKSPACE;
        case KeyboardKey::Tab: return SDLK_TAB;
        case KeyboardKey::Space: return SDLK_SPACE;
        case KeyboardKey::Exclaim: return SDLK_EXCLAIM;
        case KeyboardKey::DoubleQuote: return SDLK_DBLAPOSTROPHE;
        case KeyboardKey::Hash: return SDLK_HASH;
        case KeyboardKey::Dollar: return SDLK_DOLLAR;
        case KeyboardKey::Percent: return SDLK_PERCENT;
        case KeyboardKey::Ampersand: return SDLK_AMPERSAND;
        case KeyboardKey::Apostrophe: return SDLK_APOSTROPHE;
        case KeyboardKey::LeftParen: return SDLK_LEFTPAREN;
        case KeyboardKey::RightParen: return SDLK_RIGHTPAREN;
        case KeyboardKey::Asterisk: return SDLK_ASTERISK;
        case KeyboardKey::Plus: return SDLK_PLUS;
        case KeyboardKey::Comma: return SDLK_COMMA;
        case KeyboardKey::Minus: return SDLK_MINUS;
        case KeyboardKey::Period: return SDLK_PERIOD;
        case KeyboardKey::Slash: return SDLK_SLASH;
        case KeyboardKey::Num0: return SDLK_0;
        case KeyboardKey::Num1: return SDLK_1;
        case KeyboardKey::Num2: return SDLK_2;
        case KeyboardKey::Num3: return SDLK_3;
        case KeyboardKey::Num4: return SDLK_4;
        case KeyboardKey::Num5: return SDLK_5;
        case KeyboardKey::Num6: return SDLK_6;
        case KeyboardKey::Num7: return SDLK_7;
        case KeyboardKey::Num8: return SDLK_8;
        case KeyboardKey::Num9: return SDLK_9;
        case KeyboardKey::Colon: return SDLK_COLON;
        case KeyboardKey::Semicolon: return SDLK_SEMICOLON;
        case KeyboardKey::Less: return SDLK_LESS;
        case KeyboardKey::Equals: return SDLK_EQUALS;
        case KeyboardKey::Greater: return SDLK_GREATER;
        case KeyboardKey::Question: return SDLK_QUESTION;
        case KeyboardKey::At: return SDLK_AT;
        case KeyboardKey::LeftBracket: return SDLK_LEFTBRACKET;
        case KeyboardKey::Backslash: return SDLK_BACKSLASH;
        case KeyboardKey::RightBracket: return SDLK_RIGHTBRACKET;
        case KeyboardKey::Caret: return SDLK_CARET;
        case KeyboardKey::Underscore: return SDLK_UNDERSCORE;
        case KeyboardKey::GraveAccent: return SDLK_GRAVE;
        case KeyboardKey::A: return SDLK_A;
        case KeyboardKey::B: return SDLK_B;
        case KeyboardKey::C: return SDLK_C;
        case KeyboardKey::D: return SDLK_D;
        case KeyboardKey::E: return SDLK_E;
        case KeyboardKey::F: return SDLK_F;
        case KeyboardKey::G: return SDLK_G;
        case KeyboardKey::H: return SDLK_H;
        case KeyboardKey::I: return SDLK_I;
        case KeyboardKey::J: return SDLK_J;
        case KeyboardKey::K: return SDLK_K;
        case KeyboardKey::L: return SDLK_L;
        case KeyboardKey::M: return SDLK_M;
        case KeyboardKey::N: return SDLK_N;
        case KeyboardKey::O: return SDLK_O;
        case KeyboardKey::P: return SDLK_P;
        case KeyboardKey::Q: return SDLK_Q;
        case KeyboardKey::R: return SDLK_R;
        case KeyboardKey::S: return SDLK_S;
        case KeyboardKey::T: return SDLK_T;
        case KeyboardKey::U: return SDLK_U;
        case KeyboardKey::V: return SDLK_V;
        case KeyboardKey::W: return SDLK_W;
        case KeyboardKey::X: return SDLK_X;
        case KeyboardKey::Y: return SDLK_Y;
        case KeyboardKey::Z: return SDLK_Z;
        case KeyboardKey::LeftCurlyBrace: return SDLK_LEFTBRACE;
        case KeyboardKey::Pipe: return SDLK_PIPE;
        case KeyboardKey::RightCurlyBrace: return SDLK_RIGHTBRACE;
        case KeyboardKey::Tilde: return SDLK_TILDE;
        case KeyboardKey::Delete: return SDLK_DELETE;
        case KeyboardKey::PlusMinus: return SDLK_PLUSMINUS;
        case KeyboardKey::CapsLock: return SDLK_CAPSLOCK;
        case KeyboardKey::F1: return SDLK_F1;
        case KeyboardKey::F2: return SDLK_F2;
        case KeyboardKey::F3: return SDLK_F3;
        case KeyboardKey::F4: return SDLK_F4;
        case KeyboardKey::F5: return SDLK_F5;
        case KeyboardKey::F6: return SDLK_F6;
        case KeyboardKey::F7: return SDLK_F7;
        case KeyboardKey::F8: return SDLK_F8;
        case KeyboardKey::F9: return SDLK_F9;
        case KeyboardKey::F10: return SDLK_F10;
        case KeyboardKey::F11: return SDLK_F11;
        case KeyboardKey::F12: return SDLK_F12;
        case KeyboardKey::PrintScreen: return SDLK_PRINTSCREEN;
        case KeyboardKey::ScrollLock: return SDLK_SCROLLLOCK;
        case KeyboardKey::Pause: return SDLK_PAUSE;
        case KeyboardKey::Insert: return SDLK_INSERT;
        case KeyboardKey::Home: return SDLK_HOME;
        case KeyboardKey::PageUp: return SDLK_PAGEUP;
        case KeyboardKey::End: return SDLK_END;
        case KeyboardKey::PageDown: return SDLK_PAGEDOWN;
        case KeyboardKey::RightArrow: return SDLK_RIGHT;
        case KeyboardKey::LeftArrow: return SDLK_LEFT;
        case KeyboardKey::DownArrow: return SDLK_DOWN;
        case KeyboardKey::UpArrow: return SDLK_UP;
        case KeyboardKey::NumLock: return SDLK_NUMLOCKCLEAR;
        case KeyboardKey::KeypadDivide: return SDLK_KP_DIVIDE;
        case KeyboardKey::KeypadMultiply: return SDLK_KP_MULTIPLY;
        case KeyboardKey::KeypadMinus: return SDLK_KP_MINUS;
        case KeyboardKey::KeypadPlus: return SDLK_KP_PLUS;
        case KeyboardKey::KeypadEnter: return SDLK_KP_ENTER;
        case KeyboardKey::Keypad1: return SDLK_KP_1;
        case KeyboardKey::Keypad2: return SDLK_KP_2;
        case KeyboardKey::Keypad3: return SDLK_KP_3;
        case KeyboardKey::Keypad4: return SDLK_KP_4;
        case KeyboardKey::Keypad5: return SDLK_KP_5;
        case KeyboardKey::Keypad6: return SDLK_KP_6;
        case KeyboardKey::Keypad7: return SDLK_KP_7;
        case KeyboardKey::Keypad8: return SDLK_KP_8;
        case KeyboardKey::Keypad9: return SDLK_KP_9;
        case KeyboardKey::Keypad0: return SDLK_KP_0;
        case KeyboardKey::KeypadPeriod: return SDLK_KP_PERIOD;
        case KeyboardKey::Application: return SDLK_APPLICATION;
        case KeyboardKey::Power: return SDLK_POWER;
        case KeyboardKey::KeypadEquals: return SDLK_KP_EQUALS;
        case KeyboardKey::F13: return SDLK_F13;
        case KeyboardKey::F14: return SDLK_F14;
        case KeyboardKey::F15: return SDLK_F15;
        case KeyboardKey::F16: return SDLK_F16;
        case KeyboardKey::F17: return SDLK_F17;
        case KeyboardKey::F18: return SDLK_F18;
        case KeyboardKey::F19: return SDLK_F19;
        case KeyboardKey::F20: return SDLK_F10;
        case KeyboardKey::F21: return SDLK_F21;
        case KeyboardKey::F22: return SDLK_F22;
        case KeyboardKey::F23: return SDLK_F23;
        case KeyboardKey::F24: return SDLK_F24;
        case KeyboardKey::Execute: return SDLK_EXECUTE;
        case KeyboardKey::Help: return SDLK_HELP;
        case KeyboardKey::Menu: return SDLK_MENU;
        case KeyboardKey::Select: return SDLK_SELECT;
        case KeyboardKey::Stop: return SDLK_STOP;
        case KeyboardKey::Again: return SDLK_AGAIN;
        case KeyboardKey::Undo: return SDLK_UNDO;
        case KeyboardKey::Cut: return SDLK_CUT;
        case KeyboardKey::Copy: return SDLK_COPY;
        case KeyboardKey::Paste: return SDLK_PASTE;
        case KeyboardKey::Find: return SDLK_FIND;
        case KeyboardKey::Mute: return SDLK_MUTE;
        case KeyboardKey::VolumeUp: return SDLK_VOLUMEUP;
        case KeyboardKey::VolumeDown: return SDLK_VOLUMEDOWN;
        case KeyboardKey::KeypadComma: return SDLK_KP_COMMA;
        case KeyboardKey::KeypadEqualsAS400: return SDLK_KP_EQUALSAS400;
        case KeyboardKey::AltErase: return SDLK_ALTERASE;
        case KeyboardKey::SysReq: return SDLK_SYSREQ;
        case KeyboardKey::Cancel: return SDLK_CANCEL;
        case KeyboardKey::Clear: return SDLK_CLEAR;
        case KeyboardKey::Prior: return SDLK_PRIOR;
        case KeyboardKey::Return2: return SDLK_RETURN2;
        case KeyboardKey::Separator: return SDLK_SEPARATOR;
        case KeyboardKey::Out: return SDLK_OUT;
        case KeyboardKey::Oper: return SDLK_OPER;
        case KeyboardKey::ClearAgain: return SDLK_CLEARAGAIN;
        case KeyboardKey::CrSel: return SDLK_CRSEL;
        case KeyboardKey::ExSel: return SDLK_EXSEL;
        case KeyboardKey::Keypad00: return SDLK_KP_00;
        case KeyboardKey::Keypad000: return SDLK_KP_000;
        case KeyboardKey::ThousandsSeparator: return SDLK_THOUSANDSSEPARATOR;
        case KeyboardKey::DecimalSeparator: return SDLK_DECIMALSEPARATOR;
        case KeyboardKey::CurrencyUnit: return SDLK_CURRENCYUNIT;
        case KeyboardKey::CurrencySubunit: return SDLK_CURRENCYSUBUNIT;
        case KeyboardKey::KeypadLeftParen: return SDLK_KP_LEFTPAREN;
        case KeyboardKey::KeypadRightParen: return SDLK_KP_RIGHTPAREN;
        case KeyboardKey::KeypadLeftBrace: return SDLK_KP_LEFTBRACE;
        case KeyboardKey::KeypadRightBrace: return SDLK_KP_RIGHTBRACE;
        case KeyboardKey::KeypadTab: return SDLK_KP_TAB;
        case KeyboardKey::KeypadBackspace: return SDLK_KP_BACKSPACE;
        case KeyboardKey::KeypadA: return SDLK_KP_A;
        case KeyboardKey::KeypadB: return SDLK_KP_B;
        case KeyboardKey::KeypadC: return SDLK_KP_C;
        case KeyboardKey::KeypadD: return SDLK_KP_D;
        case KeyboardKey::KeypadE: return SDLK_KP_E;
        case KeyboardKey::KeypadF: return SDLK_KP_F;
        case KeyboardKey::KeypadXor: return SDLK_KP_XOR;
        case KeyboardKey::KeypadPower: return SDLK_KP_POWER;
        case KeyboardKey::KeypadPercent: return SDLK_KP_PERCENT;
        case KeyboardKey::KeypadLess: return SDLK_KP_LESS;
        case KeyboardKey::KeypadGreater: return SDLK_KP_GREATER;
        case KeyboardKey::KeypadAmpersand: return SDLK_KP_AMPERSAND;
        case KeyboardKey::KeypadDoubleAmpersand: return SDLK_KP_DBLAMPERSAND;
        case KeyboardKey::KeypadVerticalBar: return SDLK_KP_VERTICALBAR;
        case KeyboardKey::KeypadDoubleVerticalBar: return SDLK_KP_DBLVERTICALBAR;
        case KeyboardKey::KeypadColon: return SDLK_KP_COLON;
        case KeyboardKey::KeypadHash: return SDLK_KP_HASH;
        case KeyboardKey::KeypadSpace: return SDLK_KP_SPACE;
        case KeyboardKey::KeypadAt: return SDLK_KP_AT;
        case KeyboardKey::KeypadExclam: return SDLK_KP_EXCLAM;
        case KeyboardKey::KeypadMemStore: return SDLK_KP_MEMSTORE;
        case KeyboardKey::KeypadMemRecall: return SDLK_KP_MEMRECALL;
        case KeyboardKey::KeypadMemClear: return SDLK_KP_MEMCLEAR;
        case KeyboardKey::KeypadMemAdd: return SDLK_KP_MEMADD;
        case KeyboardKey::KeypadMemSubtract: return SDLK_KP_MEMSUBTRACT;
        case KeyboardKey::KeypadMemMultiply: return SDLK_KP_MEMMULTIPLY;
        case KeyboardKey::KeypadMemDivide: return SDLK_KP_MEMDIVIDE;
        case KeyboardKey::KeypadPlusMinus: return SDLK_KP_PLUSMINUS;
        case KeyboardKey::KeypadClear: return SDLK_KP_CLEAR;
        case KeyboardKey::KeypadClearEntry: return SDLK_KP_CLEARENTRY;
        case KeyboardKey::KeypadBinary: return SDLK_KP_BINARY;
        case KeyboardKey::KeypadOctal: return SDLK_KP_OCTAL;
        case KeyboardKey::KeypadDecimal: return SDLK_KP_DECIMAL;
        case KeyboardKey::KeypadHexadecimal: return SDLK_KP_HEXADECIMAL;
        case KeyboardKey::LeftControl: return SDLK_LCTRL;
        case KeyboardKey::LeftShift: return SDLK_LSHIFT;
        case KeyboardKey::LeftAlt: return SDLK_LALT;
        case KeyboardKey::LeftGUI: return SDLK_LGUI;
        case KeyboardKey::RightControl: return SDLK_RCTRL;
        case KeyboardKey::RightShift: return SDLK_RSHIFT;
        case KeyboardKey::RightAlt: return SDLK_RALT;
        case KeyboardKey::RightGUI: return SDLK_RGUI;
        case KeyboardKey::Mode: return SDLK_MODE;
        case KeyboardKey::Sleep: return SDLK_SLEEP;
        case KeyboardKey::Wake: return SDLK_WAKE;
        case KeyboardKey::AudioNext: return SDLK_MEDIA_NEXT_TRACK;
        case KeyboardKey::AudioPrev: return SDLK_MEDIA_PREVIOUS_TRACK;
        case KeyboardKey::AudioPlay: return SDLK_MEDIA_PLAY;
        case KeyboardKey::AudioPause: return SDLK_MEDIA_PAUSE;
        case KeyboardKey::AudioRecord: return SDLK_MEDIA_RECORD;
        case KeyboardKey::AudioFastForward: return SDLK_MEDIA_FAST_FORWARD;
        case KeyboardKey::AudioRewind: return SDLK_MEDIA_REWIND;
        case KeyboardKey::AudioNextTrack: return SDLK_MEDIA_NEXT_TRACK;
        case KeyboardKey::AudioPreviousTrack: return SDLK_MEDIA_PREVIOUS_TRACK;
        case KeyboardKey::AudioStop: return SDLK_MEDIA_STOP;
        case KeyboardKey::AudioEject: return SDLK_MEDIA_EJECT;
        case KeyboardKey::AudioPlayPause: return SDLK_MEDIA_PLAY_PAUSE;
        case KeyboardKey::AudioSelect: return SDLK_MEDIA_SELECT;
        case KeyboardKey::ACNew: return SDLK_AC_NEW;
        case KeyboardKey::ACOpen: return SDLK_AC_OPEN;
        case KeyboardKey::ACClose: return SDLK_AC_CLOSE;
        case KeyboardKey::ACExit: return SDLK_AC_EXIT;
        case KeyboardKey::ACSave: return SDLK_AC_SAVE;
        case KeyboardKey::ACPrint: return SDLK_AC_PRINT;
        case KeyboardKey::ACProperties: return SDLK_AC_PROPERTIES;
        case KeyboardKey::ACSearch: return SDLK_AC_SEARCH;
        case KeyboardKey::ACHome: return SDLK_AC_HOME;
        case KeyboardKey::ACBack: return SDLK_AC_BACK;
        case KeyboardKey::ACForward: return SDLK_AC_FORWARD;
        case KeyboardKey::ACStop: return SDLK_AC_STOP;
        case KeyboardKey::ACRefresh: return SDLK_AC_REFRESH;
        case KeyboardKey::ACBookmarks: return SDLK_AC_BOOKMARKS;
        case KeyboardKey::SoftLeft: return SDLK_SOFTLEFT;
        case KeyboardKey::SoftRight: return SDLK_SOFTRIGHT;
        case KeyboardKey::Call: return SDLK_CALL;
        case KeyboardKey::EndCall: return SDLK_ENDCALL;
        default: return SDLK_UNKNOWN;
        }
    }
}
