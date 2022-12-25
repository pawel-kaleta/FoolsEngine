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
		//defined in Application.h to avoid "include loop"
		//inline TimeStep FrameStep();
	}
}