#pragma once

#include <SDL_keyboard.h>

#include "../../Nebula/Core/Input.h"

namespace Nebula {
	/**
	 * Windows Input polling.
	 */
	class WindowsInput : public Input
	{
	public:
		WindowsInput()
		{
			int size;
			m_KeyboardState = SDL_GetKeyboardState(nullptr);
		}

	protected:
		virtual bool IsKeyPressedImpl(int keycode) override;

		virtual bool IsMouseButtonPressedImpl(int button) override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
		virtual std::pair<float, float> GetMousePositionImpl() override;

	private:
		const bool* m_KeyboardState;
	};

}
