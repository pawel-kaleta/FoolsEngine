#pragma once

#include "FoolsEngine/Core.h"
#include <chrono>
#include <thread>
#include <mutex>
#include <fstream>

namespace fe
{
	struct ProfileResult
	{
		const char* name;
		std::thread::id ThreadID;

		std::chrono::duration<double, std::micro> Start;
		std::chrono::microseconds DurationElapsed;
	};

	class Timer
	{
	public:
		Timer(const char* name)
			: m_Name(name), m_Stopped(false)
		{
			m_StartTimepoint = std::chrono::high_resolution_clock::now();
		}
		~Timer();
		
		void Stop();

	private:
		const char* m_Name;
		bool m_Stopped;
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
	};

	class Profiler
	{
	public:
		static Profiler& Get()
		{
			static Profiler SigletoneInstance;
			return SigletoneInstance;
		}
		void StartSession(const char* name, const char* filepath);
		void WriteProfile(const ProfileResult& result);
		void EndSession();
	
		Profiler(const Profiler&) = delete;
		Profiler(Profiler&&) = delete;

	private:
		Profiler()
			: m_ActiveSession(false), m_SessionName(nullptr)
		{}
		~Profiler() {
			EndSession();
		}

		bool m_ActiveSession;
		const char* m_SessionName;
		std::ofstream m_OutputStream;
		std::mutex m_Mutex;

		void WriteHeader();
		void WriteFooter();
	};
}

#if FE_INTERNAL_BUILD
///////////////////////////
// MAKROS FOR PROFILEING //
///////////////////////////
#define FE_PROFILER_START_SESSION(name, filepath) fe::Profiler::Get().StartSession(name, filepath)
#define FE_PROFILER_END_SESSION() fe::Profiler::Get().EndSession()
#define FE_PROFILER_SCOPE_BEGIN(name) FE_PROFILER_SCOPE_LINE(name, __LINE__)
///////////////////////////
#define FE_PROFILER_SCOPE_LINE(name, line) fe::Timer timer##line(name);
#else
#define FE_PROFILER_START_SESSION(name, filepath)
#define FE_PROFILER_END_SESSION()
#define FE_PROFILER_SCOPE_BEGIN(name)
#endif

