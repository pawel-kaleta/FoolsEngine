#pragma once

namespace fe
{
	namespace ProfilerUtils
	{
		template <size_t N>
		struct ChangeResult
		{
			char Data[N];
		};

		template <size_t N, size_t K>
		constexpr auto CleanupOutputString(const char(&expr)[N], const char(&remove)[K])
		{
			ChangeResult<N> result = {};

			size_t srcIndex = 0;
			size_t dstIndex = 0;
			while (srcIndex < N)
			{
				size_t matchIndex = 0;
				while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && expr[srcIndex + matchIndex] == remove[matchIndex])
					matchIndex++;
				if (matchIndex == K - 1)
					srcIndex += matchIndex;
				result.Data[dstIndex++] = expr[srcIndex] == '"' ? '\'' : expr[srcIndex];
				srcIndex++;
			}
			return result;
		}
	}

	struct ProfileResult
	{
		const char*		Name;
		std::thread::id	ThreadID;

		std::chrono::duration<double, std::micro>	Start;
		std::chrono::microseconds					DurationElapsed;
	};

	class Timer
	{
	public:
		Timer(const char* name)
			: m_Name(name), m_Stopped(false) { m_StartTimepoint = std::chrono::high_resolution_clock::now(); }
		~Timer();
		
		void Stop();

	private:
		const char*	m_Name;
		bool		m_Stopped;
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
	
		Profiler(const Profiler&)	= delete;
		Profiler(Profiler&&)		= delete;

	private:
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
}


// some magical determining of best function signature identifier in a given IDE with a given compiler
// it is expected that syntax highlighting may not work properly with this!
// TO DO: needs some cleanup to remove '__cdecl' from output string
#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
	#define FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
	#define FUNC_SIG __PRETTY_FUNCTION__
#elif (defined(__FUNCSIG__) || (_MSC_VER))
	#define FUNC_SIG __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
	#define FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
	#define FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
	#define FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
	#define FUNC_SIG __func__
#else
	#define FUNC_SIG "FUNC_SIG unknown!"
#endif

#define FE_FUNC_SIG fe::ProfilerUtils::CleanupOutputString(FUNC_SIG, "__cdecl ").Data

#if FE_INTERNAL_BUILD
	#define PROFILER_EXPANDED_ARGS(name, line) fe::Timer timer##line(FE_FUNC_SIG);
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