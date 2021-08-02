#pragma once

#include "FoolsEngine\Core\InputPolling.h"

namespace fe {

	class Win10InputPolling : public InputPolling
	{
	protected:
		virtual bool IsKeyPressedNative(int keycode) override;

		virtual bool IsMouseButtonPressedNative(int button) override;
		virtual std::pair<float, float> GetMousePositionNative() override;
		virtual float GetMouseXNative() override;
		virtual float GetMouseYNative() override;
	};
}