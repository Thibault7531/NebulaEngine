#include "WindowsInput.h"

#include "Nebula/Application.h"

#include <SDL.h>

namespace Nebula {

	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(int keycode)
	{
		return SDL_GetKeyboardState(nullptr)[keycode];
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
