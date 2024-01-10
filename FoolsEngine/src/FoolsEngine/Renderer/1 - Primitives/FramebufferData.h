#pragma once

namespace fe
{
	namespace FramebufferData
	{
		struct Specification
		{
			uint32_t Width = 0, Height = 0;

			uint32_t Samples = 1;

			bool SwapChainTarget = false;
		};
	}
}