#pragma once

#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Windows.Foundation.h>

#include "Platform/Nebula/CoreSystems/Input/KeyCodes.h"
#include "Platform/Nebula/CoreSystems/Windows/Window.h"

namespace Nebula::Platform
{

    class KeyboardEventHandler
    {
    public:
        KeyboardEventHandler(Window* window);
        ~KeyboardEventHandler();

        void InitEventHandlers();

        void SetEventCallback(const std::function<void(Event&)>& callback) {m_EventCallback = callback;}

    public:
        void KeyDownHandler(winrt::Microsoft::UI::Input::InputKeyboardSource keyboard, winrt::Microsoft::UI::Input::KeyEventArgs args);
        void KeyUpHandler(winrt::Microsoft::UI::Input::InputKeyboardSource keyboard, winrt::Microsoft::UI::Input::KeyEventArgs args);
        void CharacterReceivedHandler(winrt::Microsoft::UI::Input::InputKeyboardSource keyboard, winrt::Microsoft::UI::Input::CharacterReceivedEventArgs args);

    public:
        KeyboardKey GetKeyFromVirtualKey(winrt::Windows::System::VirtualKey virtualKey);

    private:
        Window* m_Window;

        std::function<void(Event&)> m_EventCallback;

        winrt::Microsoft::UI::Input::InputKeyboardSource m_InputKeyboardSource;
    };

}
