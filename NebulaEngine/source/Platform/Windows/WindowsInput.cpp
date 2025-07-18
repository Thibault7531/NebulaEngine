#include <SDL.h>

#include "Nebula/CoreSystems/Input/Input.h"

namespace Nebula {
	// TODO: Migrate to Win32 API
	bool Input::IsKeyPressed(int keycode)
	{
		const bool* KeyboardState = SDL_GetKeyboardState(nullptr);
		return KeyboardState[SDL_GetScancodeFromKey(keycode, nullptr)];
	}

	bool Input::IsMouseButtonPressed(int button)
	{
		return SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_MASK(button);
	}

	float Input::GetMouseX()
	{
		auto[x, y] = GetMousePosition();
		return x;
	}

	float Input::GetMouseY()
	{
		auto[x, y] = GetMousePosition();
		return y;
	}

	std::pair<float, float> Input::GetMousePosition()
	{
		float posX, posY;
		SDL_GetMouseState(&posX, &posY);
		return { posX, posY };
	}

}
