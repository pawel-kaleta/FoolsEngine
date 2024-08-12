#include "FE_pch.h"

#include "Time.h"

#include <GLFW\glfw3.h>

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

		TimeStep TimePoint::operator-(const TimePoint& b) {	return TimeStep(this->m_Time - b.GetTime()); }

		TimePoint  TimePoint::operator+ (const TimeStep& b) { return  TimePoint(this->m_Time + b.GetSeconds()); }
		TimePoint  TimePoint::operator- (const TimeStep& b) { return  TimePoint(this->m_Time - b.GetSeconds()); }
		TimePoint& TimePoint::operator+=(const TimeStep& b) { this->m_Time += b.GetSeconds(); return *this; }
		TimePoint& TimePoint::operator-=(const TimeStep& b) { this->m_Time -= b.GetSeconds(); return *this; }

		TimePoint TimePoint::operator==(const TimePoint& b) { return this->m_Time == b.m_Time; }
		TimePoint TimePoint::operator!=(const TimePoint& b) { return this->m_Time != b.m_Time; }
		TimePoint TimePoint::operator> (const TimePoint& b) { return this->m_Time > b.m_Time; }
		TimePoint TimePoint::operator< (const TimePoint& b) { return this->m_Time < b.m_Time; }
		TimePoint TimePoint::operator>=(const TimePoint& b) { return this->m_Time >= b.m_Time; }
		TimePoint TimePoint::operator<=(const TimePoint& b) { return this->m_Time <= b.m_Time; }


		TimeStep  TimeStep::operator+ (const TimeStep& b) { return  TimeStep(this->m_Time + b.m_Time); }
		TimeStep  TimeStep::operator- (const TimeStep& b) { return  TimeStep(this->m_Time - b.m_Time); }
		TimeStep& TimeStep::operator+=(const TimeStep& b) { *this = TimeStep(this->m_Time + b.m_Time); return *this; }
		TimeStep& TimeStep::operator-=(const TimeStep& b) { *this = TimeStep(this->m_Time - b.m_Time); return *this; }

		TimeStep  TimeStep::operator* (const float& b) { return TimeStep(this->m_Time * b); }
		TimeStep  TimeStep::operator/ (const float& b) { return TimeStep(this->m_Time / b); }
		TimeStep& TimeStep::operator*=(const float& b) { this->m_Time *= b; return *this; }
		TimeStep& TimeStep::operator/=(const float& b) { this->m_Time /= b; return *this; }

		TimeStep TimeStep::operator==(const TimeStep& b) { return this->m_Time == b.m_Time; }
		TimeStep TimeStep::operator!=(const TimeStep& b) { return this->m_Time != b.m_Time; }
		TimeStep TimeStep::operator> (const TimeStep& b) { return this->m_Time > b.m_Time; }
		TimeStep TimeStep::operator< (const TimeStep& b) { return this->m_Time < b.m_Time; }
		TimeStep TimeStep::operator>=(const TimeStep& b) { return this->m_Time >= b.m_Time; }
		TimeStep TimeStep::operator<=(const TimeStep& b) { return this->m_Time <= b.m_Time; }

		
	}
}