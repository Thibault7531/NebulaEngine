#include "WindowsInput.h"

#include <SDL.h>

#include "Nebula/Application.h"

namespace Nebula {

	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(int keycode)
	{
		return m_KeyboardState[SDL_GetScancodeFromKey(keycode, nullptr)];
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		return SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_MASK(button);
	}

	float WindowsInput::GetMouseXImpl()
	{
		auto[x, y] = GetMousePositionImpl();
		return x;
	}

	float WindowsInput::GetMouseYImpl()
	{
		auto[x, y] = GetMousePositionImpl();
		return y;
	}

	std::pair<float, float> WindowsInput::GetMousePositionImpl()
	{
		float posX, posY;
		SDL_GetMouseState(&posX, &posY);
		return { posX, posY };
	}

}
