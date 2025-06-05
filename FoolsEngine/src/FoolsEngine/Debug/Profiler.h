#pragma once

namespace fe
{
	struct ProfileResult
	{
		const char*		Name;
		std::thread::id	ThreadID;

		std::chrono::duration<double, std::micro>	Start;
		std::chrono::microseconds					DurationElapsed;
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
	
		Profiler(const Profiler&)	= delete;
		Profiler(Profiler&&)		= delete;

	private:
		friend class Timer;
		Profiler()
			: m_ActiveSession(false), m_SessionName(nullptr) {}
		~Profiler()
			{ if(m_ActiveSession) EndSession(); }

		bool			m_ActiveSession;
		const char*		m_SessionName;
		std::ofstream	m_OutputStream; // guarded by m_Mutex
		std::mutex		m_Mutex;

		void WriteHeader();
		void WriteFooter();
	};


	class Timer
	{
	public:
		Timer(const char* name)
			: m_Name(name), m_Stopped(false) {
			if (Profiler::Get().m_ActiveSession) m_StartTimepoint = std::chrono::high_resolution_clock::now();
		}
		~Timer();

		void Stop();

	private:
		const char* m_Name;
		bool		m_Stopped;
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
	};
}

#define FE_FUNC_SIG __builtin_FUNCSIG()

#if FE_INTERNAL_BUILD
#define PROFILER_EXPANDED_ARGS(name, line) fe::Timer timer##line(name);
#define PROFILER(name, line) PROFILER_EXPANDED_ARGS(name, line)
///////////////////////////////
//   MAKROS FOR PROFILEING   //
///////////////////////////////
#define FE_PROFILER_SESSION_START(name, filepath) fe::Profiler::Get().StartSession(name, filepath)
#define FE_PROFILER_SESSION_END() fe::Profiler::Get().EndSession()
#define FE_PROFILER_FUNC() PROFILER(FE_FUNC_SIG, __LINE__)
#define FE_PROFILER_SCOPE(name) PROFILER(name, __LINE__)
///////////////////////////////

#else
#define FE_PROFILER_SESSION_START(name, filepath)
#define FE_PROFILER_SESSION_END()
#define FE_PROFILER_FUNC()
#define FE_PROFILER_SCOPE(name)
#endif