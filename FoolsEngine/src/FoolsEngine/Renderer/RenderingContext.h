#pragma once

namespace fe
{
	class RenderingContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
		virtual void ClearBuffer() = 0;
	};
}