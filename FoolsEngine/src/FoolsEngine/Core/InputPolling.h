#pragma once

#include <vector>

namespace fe {

	class InputPolling
	{
	public:
		inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedNative(keycode); };

		inline static bool IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedNative(button); };
		inline static std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePositionNative(); };
		inline static float GetMouseX() { return s_Instance->GetMouseXNative(); };
		inline static float GetMouseY() { return s_Instance->GetMouseYNative(); };

	protected:
		virtual bool IsKeyPressedNative(int keycode) = 0;

		virtual bool IsMouseButtonPressedNative(int button) = 0;
		virtual std::pair<float, float> GetMousePositionNative() = 0;
		virtual float GetMouseXNative() = 0;
		virtual float GetMouseYNative() = 0;

	private:
		static InputPolling* s_Instance;
	};
}