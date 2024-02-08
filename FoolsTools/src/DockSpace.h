#pragma once

#include <FoolsEngine.h>

namespace fe
{
	class DockSpace
	{
	public:
		void BeginDockSpace();
		void EndDockSpace();

	private:
		bool m_FullscreenOpt = true;
	};
}