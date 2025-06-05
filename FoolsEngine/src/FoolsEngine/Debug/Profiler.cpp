#include "FE_pch.h"
#include "FoolsEngine/Debug/Profiler.h"

#include "FoolsEngine/Debug/Log.h"

#ifdef FE_INTERNAL_BUILD
namespace fe
{
	void Timer::Stop()
	{
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		std::chrono::microseconds highResStart = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch();
		std::chrono::microseconds highResEnd = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch();

		ProfileResult result;
		result.Name = m_Name;
		result.ThreadID = std::this_thread::get_id();
		result.Start = std::chrono::duration<double, std::micro>{ m_StartTimepoint.time_since_epoch() };
		result.DurationElapsed = highResEnd - highResStart;

		Profiler::Get().WriteProfile(result);
	}

	Timer::~Timer()
	{
		if (!m_Stopped && Profiler::Get().m_ActiveSession)
			Stop();
	}

	void Profiler::StartSession(const char* name, const char* filepath = "Logs/profiler.json")
	{
		if (m_ActiveSession)
		{
			if (Log::GetCoreLogger()) // if some fool deside to evoke this function before Log::Init()
				FE_LOG_CORE_ERROR("Profiler::StartSession('{0}') with already active session: '{1}'.", name, m_ActiveSession);

			EndSession(); // Simultaneous profileing with more then 1 session may cause strange behaviours
		}

		std::lock_guard lock(m_Mutex);

		m_OutputStream.open(filepath);

		if (m_OutputStream.is_open())
		{
			m_ActiveSession = true;
			m_SessionName = name;
			WriteHeader();

			if (Log::GetCoreLogger()) // if some fool deside to evoke this function before Log::Init()
				FE_LOG_CORE_INFO("Profiler session '{0}' started.", name);
		}
		else
		{
			if (Log::GetCoreLogger()) // if some fool deside to evoke this function before Log::Init()
				FE_LOG_CORE_ERROR("Profiler failed to open a results stream to file '{0}'.", filepath);
		}
	}

	void Profiler::EndSession()
	{
		std::lock_guard lock(m_Mutex);
		if (m_ActiveSession)
		{
			WriteFooter();
			m_OutputStream.close();
			m_ActiveSession = false;

			if (Log::GetCoreLogger()) // if some fool deside to evoke this function before Log::Init()
				FE_LOG_CORE_INFO("Profiler session '{0}' closed.", m_SessionName);

			m_SessionName = nullptr;
			return;
		}

		if (Log::GetCoreLogger()) // if some fool deside to evoke this function before Log::Init()
			FE_LOG_CORE_WARN("Attempt to close profiler session with no active session.");
	}

	void Profiler::WriteHeader()
	{
		m_OutputStream << "{\"otherdata\": {}, \"traceEvents\": [ {}";
		m_OutputStream.flush();
	}

	void Profiler::WriteFooter()
	{
		m_OutputStream << "]}";
		m_OutputStream.flush();
	}

	void Profiler::WriteProfile(const ProfileResult& result)
	{
		if (m_ActiveSession)
		{
			std::stringstream buffer;
			buffer << std::setprecision(3) << std::fixed;
			buffer
				<< ",{\n"
				<< "\"cat\": \"function\",\n"
				<< "\"dur\": " << result.DurationElapsed.count() << ",\n"
				<< "\"name\": \"";
			
			auto name = std::string(result.Name);
			auto beg = name.find("__cdecl ");
			if (beg != std::string::npos)
			{
				name.erase(beg, 8);
			}
				
			buffer
				<< name
				<< "\",\n"
				<< "\"ph\": \"X\",\n"
				<< "\"pid\": 0,\n"
				<< "\"tid\": " << result.ThreadID << ",\n"
				<< "\"ts\": " << result.Start.count() << ",\n"
				<< "\"args\":{}"
				<< "}";
			
			std::lock_guard lock(m_Mutex);

			m_OutputStream << buffer.str();
			m_OutputStream.flush();
		}
	}
}

#endif // FE_INTERNAL_BUILD