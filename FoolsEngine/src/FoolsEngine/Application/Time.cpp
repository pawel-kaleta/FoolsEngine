#include "FE_pch.h"
#include "Time.h"

#include <GLFW/glfw3.h>

namespace fe
{
	namespace Time
	{
		Time::TimePoint	s_LastFrameTimePoint;
		Time::TimeStep	s_LastFrameTimeStep;

		TimePoint Now()
		{
			return TimePoint((float)glfwGetTime());
			//TO DO:
			//return TimePoint( Platform::GetTime() );
		}

	}
}