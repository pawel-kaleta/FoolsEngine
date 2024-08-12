#pragma once

#include <glm/glm.hpp>

namespace fe {
	class InputPolling
	{
	public:
		InputPolling() = delete;
		InputPolling(const InputPolling&) = delete;
		InputPolling& operator=(const InputPolling&) = delete;

		static bool IsKeyPressed(int keycode);

		static bool IsMouseButtonPressed(int button);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}
