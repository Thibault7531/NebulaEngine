#pragma once

namespace Nebula {
    /**
     * Input polling interface.
     */
    class Input
    {
    public:
        /**
         * Asks if a specified key is currently pressed.
         * @param keycode Keycode of the key to poll
         * @return If the specified key is pressed
         */
        inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }

        /**
         * Asks if a specified mouse button is currently pressed.
         * @param button Mouse button to poll
         * @return If the mouse button is pressed
         */
        inline static bool IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }
        /**
         * Gets the X position of the mouse.
         * @return X position of the mouse
         */
        inline static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
        /**
         * Gets the Y position of the mouse.
         * @return Y position of the mouse
         */
        inline static float GetMouseY() { return s_Instance->GetMouseYImpl(); }
        /**
         * Gets the position of the mouse.
         * @return Position of the mouse
         */
        inline static std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }
    protected:
        virtual bool IsKeyPressedImpl(int keycode) = 0;

        virtual bool IsMouseButtonPressedImpl(int button) = 0;
        virtual float GetMouseXImpl() = 0;
        virtual float GetMouseYImpl() = 0;
        virtual std::pair<float, float> GetMousePositionImpl() = 0;
    private:
        static Input* s_Instance;
    };

}
