#include "EventHandler.h"

#include <linux/input-event-codes.h>

#include "Platform/Nebula/CoreSystems/Core/Log.h"
#include "Platform/Nebula/CoreSystems/Events/KeyEvent.h"
#include "Platform/Nebula/CoreSystems/Events/MouseEvent.h"
#include "Platform/Nebula/CoreSystems/Events/WindowEvents.h"
#include "Platform/Nebula/CoreSystems/Windows/Window.h"
#include "Platform/Nebula/CoreSystems/Windows/WindowManager.h"

namespace Nebula::Platform
{
    bool EventHandler::OnEvent(void* data, SDL_Event* SDLEvent)
    {
        SDL_Window* nativeWindow = SDL_GetWindowFromEvent(SDLEvent);
        if (!nativeWindow) return true;
        Ref<Window> window = WindowManager::Get().GetWindowFromHandle(nativeWindow);
        if (!window) return true;
        std::function<void(Event&)> callback = window->GetEventCallback();
        if (!callback) return true;

        switch (SDLEvent->type)
        {

        case SDL_EVENT_DISPLAY_ADDED:
        case SDL_EVENT_DISPLAY_REMOVED:
        case SDL_EVENT_DISPLAY_MOVED:
            {
                MonitorsChangedEvent event{};
                event.WindowID = window->GetID();
                callback(event);

                return false;
            }

        case SDL_EVENT_WINDOW_MOVED:
            {
                window->GetProperties().x = SDLEvent->window.data1;
                window->GetProperties().y = SDLEvent->window.data2;

                WindowMovedEvent event{window->GetProperties().x, window->GetProperties().y};
                event.WindowID = window->GetID();
                callback(event);

                return false;
            }

        case SDL_EVENT_WINDOW_RESIZED:
            {
                window->GetProperties().Width = SDLEvent->window.data1;
                window->GetProperties().Height = SDLEvent->window.data2;

                WindowResizedEvent event{window->GetProperties().Width, window->GetProperties().Height};
                event.WindowID = window->GetID();
                callback(event);

                return false;
            }

        case SDL_EVENT_WINDOW_MINIMIZED:
            {
                window->GetProperties().Minimized = true;
                window->GetProperties().Maximized = false;

                WindowMinimizedEvent event{};
                event.WindowID = window->GetID();
                callback(event);

                return false;
            }

        case SDL_EVENT_WINDOW_MAXIMIZED:
            {
                window->GetProperties().Minimized = false;
                window->GetProperties().Maximized = true;

                WindowMaximizedEvent event{};
                event.WindowID = window->GetID();
                callback(event);

                return false;
            }

        case SDL_EVENT_WINDOW_RESTORED:
            {
                window->GetProperties().Minimized = false;
                window->GetProperties().Maximized = false;

                WindowRestoredEvent event{};
                event.WindowID = window->GetID();
                callback(event);

                return false;
            }

        case SDL_EVENT_WINDOW_MOUSE_ENTER:
            {
                MouseEnteredEvent event{};
                event.WindowID = window->GetID();
                callback(event);

                return false;
            }

        case SDL_EVENT_WINDOW_MOUSE_LEAVE:
            {
                MouseLeftEvent event{};
                event.WindowID = window->GetID();
                callback(event);

                return false;
            }

        case SDL_EVENT_WINDOW_FOCUS_GAINED:
            {
                WindowFocusedEvent event{};
                event.WindowID = window->GetID();
                callback(event);

                return false;
            }

        case SDL_EVENT_WINDOW_FOCUS_LOST:
            {
                WindowLostFocusEvent event{};
                event.WindowID = window->GetID();
                callback(event);

                return false;
            }

        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            {
                WindowClosedEvent event{};
                event.WindowID = window->GetID();
                callback(event);

                return false;
            }

        case SDL_EVENT_KEY_DOWN:
            {
                KeyPressedEvent event{GetKeyboardKeyFromSDLKeycode(SDLEvent->key.key), static_cast<uint32_t>(SDLEvent->key.repeat ? 1 : 0)};
                event.WindowID = window->GetID();
                callback(event);

                return false;
            }

        case SDL_EVENT_KEY_UP:
            {
                KeyReleasedEvent event{GetKeyboardKeyFromSDLKeycode(SDLEvent->key.key)};
                event.WindowID = window->GetID();
                callback(event);

                return false;
            }

        case SDL_EVENT_TEXT_INPUT:
            {
                KeyTypedEvent event{SDLEvent->text.text[0]};
                event.WindowID = window->GetID();
                callback(event);

                return false;
            }

        case SDL_EVENT_MOUSE_MOTION:
            {
                MouseMovedEvent event{SDLEvent->motion.x, SDLEvent->motion.y};
                event.WindowID = window->GetID();
                callback(event);

                return false;
            }

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                MouseButtonPressedEvent event{GetMouseButtonFromSDLButtoncode(SDLEvent->button.button)};
                event.WindowID = window->GetID();
                callback(event);

                return false;
            }

        case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                MouseButtonReleasedEvent event{GetMouseButtonFromSDLButtoncode(SDLEvent->button.button)};
                event.WindowID = window->GetID();
                callback(event);

                return false;
            }

        case SDL_EVENT_MOUSE_WHEEL:
            {
                MouseScrolledEvent event{SDLEvent->wheel.x, SDLEvent->wheel.y};
                event.WindowID = window->GetID();
                callback(event);

                return false;
            }

        default: return true;
        }
    }

    SDL_HitTestResult EventHandler::OnHitTest(SDL_Window* win, const SDL_Point* area, void* data)
    {
        Ref<Window> window = WindowManager::Get().GetWindowFromHandle(win);
        if (!window) return SDL_HITTEST_NORMAL;
        std::function<HitTest(int32_t, int32_t)> hittest = window->GetWindowHitTest();
        if (!hittest) return SDL_HITTEST_NORMAL;
        HitTest result = hittest(area->x, area->y);

        switch (result)
        {
        case HitTest::None: return SDL_HITTEST_NORMAL;
        case HitTest::Client: return SDL_HITTEST_NORMAL;
        case HitTest::TitleBar: return SDL_HITTEST_DRAGGABLE;
        case HitTest::ResizeTopLeft: return SDL_HITTEST_RESIZE_TOPLEFT;
        case HitTest::ResizeTop: return SDL_HITTEST_RESIZE_TOP;
        case HitTest::ResizeTopRight: return SDL_HITTEST_RESIZE_TOPRIGHT;
        case HitTest::ResizeRight: return SDL_HITTEST_RESIZE_RIGHT;
        case HitTest::ResizeBottomRight: return SDL_HITTEST_RESIZE_BOTTOMRIGHT;
        case HitTest::ResizeBottom: return SDL_HITTEST_RESIZE_BOTTOM;
        case HitTest::ResizeBottomLeft: return SDL_HITTEST_RESIZE_BOTTOMLEFT;
        case HitTest::ResizeLeft: return SDL_HITTEST_RESIZE_LEFT;
        default: return SDL_HITTEST_NORMAL;
        }
    }

    KeyboardKey EventHandler::GetKeyboardKeyFromSDLKeycode(SDL_Keycode keycode)
    {
        switch (keycode)
        {
        case SDLK_UNKNOWN: return KeyboardKey::Unknown;
        case SDLK_RETURN: return KeyboardKey::Return;
        case SDLK_ESCAPE: return KeyboardKey::Escape;
        case SDLK_BACKSPACE: return KeyboardKey::Backspace;
        case SDLK_TAB: return KeyboardKey::Tab;
        case SDLK_SPACE: return KeyboardKey::Space;
        case SDLK_EXCLAIM: return KeyboardKey::Exclaim;
        case SDLK_DBLAPOSTROPHE: return KeyboardKey::DoubleQuote;
        case SDLK_HASH: return KeyboardKey::Hash;
        case SDLK_DOLLAR: return KeyboardKey::Dollar;
        case SDLK_PERCENT: return KeyboardKey::Percent;
        case SDLK_AMPERSAND: return KeyboardKey::Ampersand;
        case SDLK_APOSTROPHE: return KeyboardKey::Apostrophe;
        case SDLK_LEFTPAREN: return KeyboardKey::LeftParen;
        case SDLK_RIGHTPAREN: return KeyboardKey::RightParen;
        case SDLK_ASTERISK: return KeyboardKey::Asterisk;
        case SDLK_PLUS: return KeyboardKey::Plus;
        case SDLK_COMMA: return KeyboardKey::Comma;
        case SDLK_MINUS: return KeyboardKey::Minus;
        case SDLK_PERIOD: return KeyboardKey::Period;
        case SDLK_SLASH: return KeyboardKey::Slash;
        case SDLK_0: return KeyboardKey::Num0;
        case SDLK_1: return KeyboardKey::Num1;
        case SDLK_2: return KeyboardKey::Num2;
        case SDLK_3: return KeyboardKey::Num3;
        case SDLK_4: return KeyboardKey::Num4;
        case SDLK_5: return KeyboardKey::Num5;
        case SDLK_6: return KeyboardKey::Num6;
        case SDLK_7: return KeyboardKey::Num7;
        case SDLK_8: return KeyboardKey::Num8;
        case SDLK_9: return KeyboardKey::Num9;
        case SDLK_COLON: return KeyboardKey::Colon;
        case SDLK_SEMICOLON: return KeyboardKey::Semicolon;
        case SDLK_LESS: return KeyboardKey::Less;
        case SDLK_EQUALS: return KeyboardKey::Equals;
        case SDLK_GREATER: return KeyboardKey::Greater;
        case SDLK_QUESTION: return KeyboardKey::Question;
        case SDLK_AT: return KeyboardKey::At;
        case SDLK_LEFTBRACKET: return KeyboardKey::LeftBracket;
        case SDLK_BACKSLASH: return KeyboardKey::Backslash;
        case SDLK_RIGHTBRACKET: return KeyboardKey::RightBracket;
        case SDLK_CARET: return KeyboardKey::Caret;
        case SDLK_UNDERSCORE: return KeyboardKey::Underscore;
        case SDLK_GRAVE: return KeyboardKey::GraveAccent;
        case SDLK_A: return KeyboardKey::A;
        case SDLK_B: return KeyboardKey::B;
        case SDLK_C: return KeyboardKey::C;
        case SDLK_D: return KeyboardKey::D;
        case SDLK_E: return KeyboardKey::E;
        case SDLK_F: return KeyboardKey::F;
        case SDLK_G: return KeyboardKey::G;
        case SDLK_H: return KeyboardKey::H;
        case SDLK_I: return KeyboardKey::I;
        case SDLK_J: return KeyboardKey::J;
        case SDLK_K: return KeyboardKey::K;
        case SDLK_L: return KeyboardKey::L;
        case SDLK_M: return KeyboardKey::M;
        case SDLK_N: return KeyboardKey::N;
        case SDLK_O: return KeyboardKey::O;
        case SDLK_P: return KeyboardKey::P;
        case SDLK_Q: return KeyboardKey::Q;
        case SDLK_R: return KeyboardKey::R;
        case SDLK_S: return KeyboardKey::S;
        case SDLK_T: return KeyboardKey::T;
        case SDLK_U: return KeyboardKey::U;
        case SDLK_V: return KeyboardKey::V;
        case SDLK_W: return KeyboardKey::W;
        case SDLK_X: return KeyboardKey::X;
        case SDLK_Y: return KeyboardKey::Y;
        case SDLK_Z: return KeyboardKey::Z;
        case SDLK_LEFTBRACE: return KeyboardKey::LeftCurlyBrace;
        case SDLK_PIPE: return KeyboardKey::Pipe;
        case SDLK_RIGHTBRACE: return KeyboardKey::RightCurlyBrace;
        case SDLK_TILDE: return KeyboardKey::Tilde;
        case SDLK_DELETE: return KeyboardKey::Delete;
        case SDLK_PLUSMINUS: return KeyboardKey::PlusMinus;
        case SDLK_CAPSLOCK: return KeyboardKey::CapsLock;
        case SDLK_F1: return KeyboardKey::F1;
        case SDLK_F2: return KeyboardKey::F2;
        case SDLK_F3: return KeyboardKey::F3;
        case SDLK_F4: return KeyboardKey::F4;
        case SDLK_F5: return KeyboardKey::F5;
        case SDLK_F6: return KeyboardKey::F6;
        case SDLK_F7: return KeyboardKey::F7;
        case SDLK_F8: return KeyboardKey::F8;
        case SDLK_F9: return KeyboardKey::F9;
        case SDLK_F10: return KeyboardKey::F10;
        case SDLK_F11: return KeyboardKey::F11;
        case SDLK_F12: return KeyboardKey::F12;
        case SDLK_PRINTSCREEN: return KeyboardKey::PrintScreen;
        case SDLK_SCROLLLOCK: return KeyboardKey::ScrollLock;
        case SDLK_PAUSE: return KeyboardKey::Pause;
        case SDLK_INSERT: return KeyboardKey::Insert;
        case SDLK_HOME: return KeyboardKey::Home;
        case SDLK_PAGEUP: return KeyboardKey::PageUp;
        case SDLK_END: return KeyboardKey::End;
        case SDLK_PAGEDOWN: return KeyboardKey::PageDown;
        case SDLK_RIGHT: return KeyboardKey::RightArrow;
        case SDLK_LEFT: return KeyboardKey::LeftArrow;
        case SDLK_DOWN: return KeyboardKey::DownArrow;
        case SDLK_UP: return KeyboardKey::UpArrow;
        case SDLK_NUMLOCKCLEAR: return KeyboardKey::NumLock;
        case SDLK_KP_DIVIDE: return KeyboardKey::KeypadDivide;
        case SDLK_KP_MULTIPLY: return KeyboardKey::KeypadMultiply;
        case SDLK_KP_MINUS: return KeyboardKey::KeypadMinus;
        case SDLK_KP_PLUS: return KeyboardKey::KeypadPlus;
        case SDLK_KP_ENTER: return KeyboardKey::KeypadEnter;
        case SDLK_KP_1: return KeyboardKey::Keypad1;
        case SDLK_KP_2: return KeyboardKey::Keypad2;
        case SDLK_KP_3: return KeyboardKey::Keypad3;
        case SDLK_KP_4: return KeyboardKey::Keypad4;
        case SDLK_KP_5: return KeyboardKey::Keypad5;
        case SDLK_KP_6: return KeyboardKey::Keypad6;
        case SDLK_KP_7: return KeyboardKey::Keypad7;
        case SDLK_KP_8: return KeyboardKey::Keypad8;
        case SDLK_KP_9: return KeyboardKey::Keypad9;
        case SDLK_KP_0: return KeyboardKey::Keypad0;
        case SDLK_KP_PERIOD: return KeyboardKey::KeypadDecimal;
        case SDLK_APPLICATION: return KeyboardKey::Application;
        case SDLK_POWER: return KeyboardKey::Power;
        case SDLK_KP_EQUALS: return KeyboardKey::KeypadEquals;
        case SDLK_F13: return KeyboardKey::F13;
        case SDLK_F14: return KeyboardKey::F14;
        case SDLK_F15: return KeyboardKey::F15;
        case SDLK_F16: return KeyboardKey::F16;
        case SDLK_F17: return KeyboardKey::F17;
        case SDLK_F18: return KeyboardKey::F18;
        case SDLK_F19: return KeyboardKey::F19;
        case SDLK_F20: return KeyboardKey::F20;
        case SDLK_F21: return KeyboardKey::F21;
        case SDLK_F22: return KeyboardKey::F22;
        case SDLK_F23: return KeyboardKey::F23;
        case SDLK_F24: return KeyboardKey::F24;
        case SDLK_EXECUTE: return KeyboardKey::Execute;
        case SDLK_HELP: return KeyboardKey::Help;
        case SDLK_MENU: return KeyboardKey::Menu;
        case SDLK_SELECT: return KeyboardKey::Select;
        case SDLK_STOP: return KeyboardKey::Stop;
        case SDLK_AGAIN: return KeyboardKey::Again;
        case SDLK_UNDO: return KeyboardKey::Undo;
        case SDLK_CUT: return KeyboardKey::Cut;
        case SDLK_COPY: return KeyboardKey::Copy;
        case SDLK_PASTE: return KeyboardKey::Paste;
        case SDLK_FIND: return KeyboardKey::Find;
        case SDLK_MUTE: return KeyboardKey::Mute;
        case SDLK_VOLUMEUP: return KeyboardKey::VolumeUp;
        case SDLK_VOLUMEDOWN: return KeyboardKey::VolumeDown;
        case SDLK_KP_COMMA: return KeyboardKey::KeypadComma;
        case SDLK_KP_EQUALSAS400: return KeyboardKey::KeypadEqualsAS400;
        case SDLK_ALTERASE: return KeyboardKey::AltErase;
        case SDLK_SYSREQ: return KeyboardKey::SysReq;
        case SDLK_CANCEL: return KeyboardKey::Cancel;
        case SDLK_CLEAR: return KeyboardKey::Clear;
        case SDLK_PRIOR: return KeyboardKey::Prior;
        case SDLK_RETURN2: return KeyboardKey::Return2;
        case SDLK_SEPARATOR: return KeyboardKey::Separator;
        case SDLK_OUT: return KeyboardKey::Out;
        case SDLK_OPER: return KeyboardKey::Oper;
        case SDLK_CLEARAGAIN: return KeyboardKey::ClearAgain;
        case SDLK_CRSEL: return KeyboardKey::CrSel;
        case SDLK_EXSEL: return KeyboardKey::ExSel;
        case SDLK_KP_00: return KeyboardKey::Keypad00;
        case SDLK_KP_000: return KeyboardKey::Keypad000;
        case SDLK_THOUSANDSSEPARATOR: return KeyboardKey::ThousandsSeparator;
        case SDLK_DECIMALSEPARATOR: return KeyboardKey::DecimalSeparator;
        case SDLK_CURRENCYUNIT: return KeyboardKey::CurrencyUnit;
        case SDLK_CURRENCYSUBUNIT: return KeyboardKey::CurrencySubunit;
        case SDLK_KP_LEFTPAREN: return KeyboardKey::KeypadLeftParen;
        case SDLK_KP_RIGHTPAREN: return KeyboardKey::KeypadRightParen;
        case SDLK_KP_LEFTBRACE: return KeyboardKey::KeypadLeftBrace;
        case SDLK_KP_RIGHTBRACE: return KeyboardKey::KeypadRightBrace;
        case SDLK_KP_TAB: return KeyboardKey::KeypadTab;
        case SDLK_KP_BACKSPACE: return KeyboardKey::KeypadBackspace;
        case SDLK_KP_A: return KeyboardKey::KeypadA;
        case SDLK_KP_B: return KeyboardKey::KeypadB;
        case SDLK_KP_C: return KeyboardKey::KeypadC;
        case SDLK_KP_D: return KeyboardKey::KeypadD;
        case SDLK_KP_E: return KeyboardKey::KeypadE;
        case SDLK_KP_F: return KeyboardKey::KeypadF;
        case SDLK_KP_XOR: return KeyboardKey::KeypadXor;
        case SDLK_KP_POWER: return KeyboardKey::KeypadPower;
        case SDLK_KP_PERCENT: return KeyboardKey::KeypadPercent;
        case SDLK_KP_LESS: return KeyboardKey::KeypadLess;
        case SDLK_KP_GREATER: return KeyboardKey::KeypadGreater;
        case SDLK_KP_AMPERSAND: return KeyboardKey::KeypadAmpersand;
        case SDLK_KP_DBLAMPERSAND: return KeyboardKey::KeypadDoubleAmpersand;
        case SDLK_KP_VERTICALBAR: return KeyboardKey::KeypadVerticalBar;
        case SDLK_KP_DBLVERTICALBAR: return KeyboardKey::KeypadDoubleVerticalBar;
        case SDLK_KP_COLON: return KeyboardKey::KeypadColon;
        case SDLK_KP_HASH: return KeyboardKey::KeypadHash;
        case SDLK_KP_SPACE: return KeyboardKey::KeypadSpace;
        case SDLK_KP_AT: return KeyboardKey::KeypadAt;
        case SDLK_KP_EXCLAM: return KeyboardKey::KeypadExclam;
        case SDLK_KP_MEMSTORE: return KeyboardKey::KeypadMemStore;
        case SDLK_KP_MEMRECALL: return KeyboardKey::KeypadMemRecall;
        case SDLK_KP_MEMCLEAR: return KeyboardKey::KeypadMemClear;
        case SDLK_KP_MEMADD: return KeyboardKey::KeypadMemAdd;
        case SDLK_KP_MEMSUBTRACT: return KeyboardKey::KeypadMemSubtract;
        case SDLK_KP_MEMMULTIPLY: return KeyboardKey::KeypadMemMultiply;
        case SDLK_KP_MEMDIVIDE: return KeyboardKey::KeypadMemDivide;
        case SDLK_KP_PLUSMINUS: return KeyboardKey::KeypadPlusMinus;
        case SDLK_KP_CLEAR: return KeyboardKey::KeypadClear;
        case SDLK_KP_CLEARENTRY: return KeyboardKey::KeypadClearEntry;
        case SDLK_KP_BINARY: return KeyboardKey::KeypadBinary;
        case SDLK_KP_OCTAL: return KeyboardKey::KeypadOctal;
        case SDLK_KP_DECIMAL: return KeyboardKey::KeypadDecimal;
        case SDLK_KP_HEXADECIMAL: return KeyboardKey::KeypadHexadecimal;
        case SDLK_LCTRL: return KeyboardKey::LeftControl;
        case SDLK_LSHIFT: return KeyboardKey::LeftShift;
        case SDLK_LALT: return KeyboardKey::LeftAlt;
        case SDLK_LGUI: return KeyboardKey::LeftGUI;
        case SDLK_RCTRL: return KeyboardKey::RightControl;
        case SDLK_RSHIFT: return KeyboardKey::RightShift;
        case SDLK_RALT: return KeyboardKey::RightAlt;
        case SDLK_RGUI: return KeyboardKey::RightGUI;
        case SDLK_MODE: return KeyboardKey::Mode;
        case SDLK_SLEEP: return KeyboardKey::Sleep;
        case SDLK_WAKE: return KeyboardKey::Wake;
        case SDLK_MEDIA_PLAY: return KeyboardKey::AudioPlay;
        case SDLK_MEDIA_PAUSE: return KeyboardKey::AudioPause;
        case SDLK_MEDIA_RECORD: return KeyboardKey::AudioRecord;
        case SDLK_MEDIA_FAST_FORWARD: return KeyboardKey::AudioFastForward;
        case SDLK_MEDIA_REWIND: return KeyboardKey::AudioRewind;
        case SDLK_MEDIA_NEXT_TRACK: return KeyboardKey::AudioNext;
        case SDLK_MEDIA_PREVIOUS_TRACK: return KeyboardKey::AudioPrev;
        case SDLK_MEDIA_STOP: return KeyboardKey::AudioStop;
        case SDLK_MEDIA_EJECT: return KeyboardKey::AudioEject;
        case SDLK_MEDIA_PLAY_PAUSE: return KeyboardKey::AudioPlayPause;
        case SDLK_MEDIA_SELECT: return KeyboardKey::AudioSelect;
        case SDLK_AC_NEW: return KeyboardKey::ACNew;
        case SDLK_AC_OPEN: return KeyboardKey::ACOpen;
        case SDLK_AC_CLOSE: return KeyboardKey::ACClose;
        case SDLK_AC_EXIT: return KeyboardKey::ACExit;
        case SDLK_AC_SAVE: return KeyboardKey::ACSave;
        case SDLK_AC_PRINT: return KeyboardKey::ACPrint;
        case SDLK_AC_PROPERTIES: return KeyboardKey::ACProperties;
        case SDLK_AC_SEARCH: return KeyboardKey::ACSearch;
        case SDLK_AC_HOME: return KeyboardKey::ACHome;
        case SDLK_AC_BACK: return KeyboardKey::ACBack;
        case SDLK_AC_FORWARD: return KeyboardKey::ACForward;
        case SDLK_AC_STOP: return KeyboardKey::ACStop;
        case SDLK_AC_REFRESH: return KeyboardKey::ACRefresh;
        case SDLK_AC_BOOKMARKS: return KeyboardKey::ACBookmarks;
        case SDLK_SOFTLEFT: return KeyboardKey::SoftLeft;
        case SDLK_SOFTRIGHT: return KeyboardKey::SoftRight;
        case SDLK_CALL: return KeyboardKey::Call;
        case SDLK_ENDCALL: return KeyboardKey::EndCall;

        default: return KeyboardKey::Unknown;
        }
    }

    MouseButton EventHandler::GetMouseButtonFromSDLButtoncode(Uint8 button)
    {
        switch (button)
        {
            case SDL_BUTTON_LEFT: return MouseButton::Left;
            case SDL_BUTTON_MIDDLE: return MouseButton::Middle;
            case SDL_BUTTON_RIGHT: return MouseButton::Right;
            case SDL_BUTTON_X1: return MouseButton::X1;
            case SDL_BUTTON_X2: return MouseButton::X2;
            default: return MouseButton::Unknown;
        }
    }
}
