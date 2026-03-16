#pragma once

namespace fe
{
	namespace Time
	{
		class TimeStep;

		class TimePoint
		{
		public:
			TimePoint(float timeSeconds = 0.0f)
				: m_Time(timeSeconds) {}

			inline float GetTime() const { return m_Time; }

			TimeStep operator-(const TimePoint& b);

			TimePoint  operator+ (const TimeStep& b);
			TimePoint  operator- (const TimeStep& b);
			TimePoint& operator+=(const TimeStep& b);
			TimePoint& operator-=(const TimeStep& b);

			TimePoint operator==(const TimePoint& b);
			TimePoint operator!=(const TimePoint& b);
			TimePoint operator> (const TimePoint& b);
			TimePoint operator< (const TimePoint& b);
			TimePoint operator>=(const TimePoint& b);
			TimePoint operator<=(const TimePoint& b);
		private:
			float m_Time;
		};

		class TimeStep
		{
		public:
			TimeStep(float timeSeconds = 0)
				: m_Time(timeSeconds) {}
			TimeStep(const TimePoint& begin, const TimePoint& end)
				: m_Time(end.GetTime() - begin.GetTime()) {}

			inline float GetSeconds() const { return m_Time; }
			inline float GetMilliseconds() const { return m_Time * 1000; }

			TimeStep  operator+ (const TimeStep& b);
			TimeStep  operator- (const TimeStep& b);
			TimeStep& operator+=(const TimeStep& b);
			TimeStep& operator-=(const TimeStep& b);

			TimeStep  operator* (const float& b);
			TimeStep  operator/ (const float& b);
			TimeStep& operator*=(const float& b);
			TimeStep& operator/=(const float& b);

			TimeStep operator==(const TimeStep& b);
			TimeStep operator!=(const TimeStep& b);
			TimeStep operator> (const TimeStep& b);
			TimeStep operator< (const TimeStep& b);
			TimeStep operator>=(const TimeStep& b);
			TimeStep operator<=(const TimeStep& b);

		private:
			float m_Time;
		};
	
		TimePoint Now();

		inline float DeltaTime()
		{
			extern TimeStep s_LastFrameTimeStep;
			return s_LastFrameTimeStep.GetSeconds();
		}


		inline TimeStep TimePoint::operator-(const TimePoint& b) { return TimeStep(this->m_Time - b.GetTime()); }

		inline TimePoint  TimePoint::operator+ (const TimeStep& b) { return  TimePoint(this->m_Time + b.GetSeconds()); }
		inline TimePoint  TimePoint::operator- (const TimeStep& b) { return  TimePoint(this->m_Time - b.GetSeconds()); }
		inline TimePoint& TimePoint::operator+=(const TimeStep& b) { this->m_Time += b.GetSeconds(); return *this; }
		inline TimePoint& TimePoint::operator-=(const TimeStep& b) { this->m_Time -= b.GetSeconds(); return *this; }

		inline TimePoint TimePoint::operator==(const TimePoint& b) { return this->m_Time == b.m_Time; }
		inline TimePoint TimePoint::operator!=(const TimePoint& b) { return this->m_Time != b.m_Time; }
		inline TimePoint TimePoint::operator> (const TimePoint& b) { return this->m_Time > b.m_Time; }
		inline TimePoint TimePoint::operator< (const TimePoint& b) { return this->m_Time < b.m_Time; }
		inline TimePoint TimePoint::operator>=(const TimePoint& b) { return this->m_Time >= b.m_Time; }
		inline TimePoint TimePoint::operator<=(const TimePoint& b) { return this->m_Time <= b.m_Time; }


		inline TimeStep  TimeStep::operator+ (const TimeStep& b) { return  TimeStep(this->m_Time + b.m_Time); }
		inline TimeStep  TimeStep::operator- (const TimeStep& b) { return  TimeStep(this->m_Time - b.m_Time); }
		inline TimeStep& TimeStep::operator+=(const TimeStep& b) { *this = TimeStep(this->m_Time + b.m_Time); return *this; }
		inline TimeStep& TimeStep::operator-=(const TimeStep& b) { *this = TimeStep(this->m_Time - b.m_Time); return *this; }

		inline TimeStep  TimeStep::operator* (const float& b) { return TimeStep(this->m_Time * b); }
		inline TimeStep  TimeStep::operator/ (const float& b) { return TimeStep(this->m_Time / b); }
		inline TimeStep& TimeStep::operator*=(const float& b) { this->m_Time *= b; return *this; }
		inline TimeStep& TimeStep::operator/=(const float& b) { this->m_Time /= b; return *this; }

		inline TimeStep TimeStep::operator==(const TimeStep& b) { return this->m_Time == b.m_Time; }
		inline TimeStep TimeStep::operator!=(const TimeStep& b) { return this->m_Time != b.m_Time; }
		inline TimeStep TimeStep::operator> (const TimeStep& b) { return this->m_Time > b.m_Time; }
		inline TimeStep TimeStep::operator< (const TimeStep& b) { return this->m_Time < b.m_Time; }
		inline TimeStep TimeStep::operator>=(const TimeStep& b) { return this->m_Time >= b.m_Time; }
		inline TimeStep TimeStep::operator<=(const TimeStep& b) { return this->m_Time <= b.m_Time; }
	}
}