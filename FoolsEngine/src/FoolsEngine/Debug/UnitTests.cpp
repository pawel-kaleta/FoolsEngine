#include "FE_pch.h"
#include "FoolsEngine/Core/Core.h"
#include "FoolsEngine/Debug/Log.h"
#include "FoolsEngine/Debug/Asserts.h"
#include "FoolsEngine/Debug/UnitTests.h"
#include "FoolsEngine/Debug/Profiler.h"

#ifdef FE_INTERNAL_BUILD
namespace fe
{
	void Tester::LoggingTests()
	{
		FE_LOG_CORE_DEBUG("----------------------Logging tests---------------------");
		int coreLoggingLvl = Log::GetCoreLoggingLevel();
		int clientLoggingLvl = Log::GetClientLoggingLevel();
		Log::SetCoreLoggingLevel(0);
		Log::SetClientLoggingLevel(0);
		FE_LOG_CORE_FATAL("Fatal test");
		FE_LOG_FATAL("Fatal test");
		FE_LOG_CORE_ERROR("Error test");
		FE_LOG_ERROR("Error test");
		FE_LOG_CORE_WARN("Warn test");
		FE_LOG_WARN("Warn test");
		FE_LOG_CORE_INFO("Info test");
		FE_LOG_INFO("Info test");
		FE_LOG_CORE_DEBUG("Debug test");
		FE_LOG_DEBUG("Debug test");
		FE_LOG_CORE_TRACE("Trace test");
		FE_LOG_TRACE("Trace test");
		Log::SetCoreLoggingLevel(5);
		Log::SetCoreLoggingLevel(10);
		Log::SetClientLoggingLevel(4);
		Log::SetClientLoggingLevel(11);
		Log::SetCoreLoggingLevel(coreLoggingLvl);
		Log::SetClientLoggingLevel(clientLoggingLvl);
	}
	
	/*
	class TestEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(Custom)
		EVENT_CLASS_CATEGORY(Custom)

		int m_TestData;

		TestEvent(int testData)
			: m_TestData(testData)
		{ }
	};

	
	void TestEventHandler(Event& event)
	{
		FE_LOG_CORE_DEBUG("{0}", event);
	}
	*/
	/*
	void EventsTests()
	{
		FE_LOG_CORE_DEBUG("----------------------Events tests----------------------");

		TestEvent event = TestEvent(99);

		if ( "Custom" == (event.GetName()) )
			FE_LOG_CORE_INFO("event.GetName() test: PASSED" );
		else
			FE_LOG_CORE_ERROR("event.GetName() test: FAILED");

		if (event.GetEventType() == EventType::Custom)
			FE_LOG_CORE_INFO("event.GetEventType() test: PASSED");
		else
			FE_LOG_CORE_ERROR("event.GetEventType() test: FAILED");

		if (event.GetStaticType() == EventType::Custom)
			FE_LOG_CORE_INFO("event.GetStaticType() test: PASSED");
		else
			FE_LOG_CORE_ERROR("event.GetStaticType() test: FAILED");

		if (event.GetCategoryFlags() == EventCategory::Custom)
			FE_LOG_CORE_INFO("event.GetCategoryFlags() test: PASSED");
		else
			FE_LOG_CORE_ERROR("event.GetCategoryFlags() test: FAILED");

		if (event.GetStaticCategoryFlags() == EventCategory::Custom)
			FE_LOG_CORE_INFO("event.GetStaticCategoryFlags() test: PASSED");
		else
			FE_LOG_CORE_ERROR("event.GetStaticCategoryFlags() test: FAILED");

		if (event.m_TestData == 99)
			FE_LOG_CORE_INFO("event.m_TestData test: PASSED");
		else
			FE_LOG_CORE_ERROR("event.m_TestData test: FAILED");

		if (event.IsInCategory(EventCategory::EngineSys))
			FE_LOG_CORE_ERROR("event.IsInCategory() test_1: FAILED");
		else
			FE_LOG_CORE_INFO("event.IsInCategory() test_1: PASSED");

		if (event.IsInCategory(EventCategory::Custom))
			FE_LOG_CORE_INFO("event.IsInCategory() test_2: PASSED");
		else
			FE_LOG_CORE_ERROR("event.IsInCategory() test_2: FAILED");

		FE_LOG_CORE_DEBUG("----------------------Events tests----------------------");
	}

	bool funtionRunned = false;
	void TestEventHandler(Event& event)
	{
		funtionRunned = true;
	}

	void EventsDispachersTests()
	{
		FE_LOG_CORE_DEBUG("-----------------Events Dispachers tests----------------");

		TestEvent event = TestEvent(99);
		std::function< void(Event&) > f = TestEventHandler;

		f(event);
		if (funtionRunned)
			FE_LOG_CORE_INFO("Handling Events test: PASSED");
		else
			FE_LOG_CORE_ERROR("Handling Events test: FAILED");



		std::shared_ptr<EventDispacherBlocking> testBlockingDispacher = std::make_shared<EventDispacherBlocking>();
		//testBlockingDispacher->AddSubscription(std::make_shared<EventSubscryption>(f, EventType::Custom));

		FE_LOG_CORE_DEBUG("-----------------Events Dispachers tests----------------");
	}
	*/

	void profilingTestFunction(int limit)
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_INFO("Profiler Scope Function Begin: PASSED");
		for (int i = 0; i < limit; i++) {}
	}

	void Tester::ProfilingTests()
	{
		FE_LOG_CORE_DEBUG("---------------------Profiler tests---------------------");
		FE_PROFILER_SESSION_START("TEST", "Logs/UNIT_TESTS_profiler.json");
		FE_LOG_CORE_INFO("Profiler Start Session: PASSED");
		int limit = 200000000;
		profilingTestFunction(limit);
		FE_LOG_CORE_INFO("Profiler Timer Stop: PASSED");
		{
			FE_PROFILER_SCOPE("Scope_lvl_1()");
			FE_LOG_CORE_INFO("Profiler Scope Name Begin: PASSED");
			for (int i = 0; i < limit; i++) {}
			{
				FE_PROFILER_SCOPE("Scope_lvl_2()");
				for (int i = 0; i < limit; i++) {}
				{
					FE_PROFILER_SCOPE("Scope_lvl_3()");
					for (int i = 0; i < limit; i++) {}
				}
				for (int i = 0; i < limit; i++) {}
			}
			for (int i = 0; i < limit; i++) {}
			{
				FE_PROFILER_SCOPE("Scope_lvl_2()");
				for (int i = 0; i < limit; i++) {}
				{
					FE_PROFILER_SCOPE("Scope_lvl_3()");
					for (int i = 0; i < limit; i++) {}
				}
				for (int i = 0; i < limit; i++) {}
			}
			for (int i = 0; i < limit; i++) {}
			{
				FE_PROFILER_SCOPE("Scope_lvl_2()");
				for (int i = 0; i < limit; i++) {}
				{
					FE_PROFILER_SCOPE("Scope_lvl_3()");
					for (int i = 0; i < limit; i++) {}
				}
				for (int i = 0; i < limit; i++) {}
			}
			for (int i = 0; i < limit; i++) {}
		}
		for (int i = 0; i < limit; i++) {}
		FE_PROFILER_SESSION_END();
		FE_LOG_CORE_INFO("Profiler End Session: PASSED");
		FE_LOG_CORE_INFO("Profiler: PASSED");
	}

	void Tester::AssertionTests()
	{
		FE_LOG_CORE_DEBUG("--------------------Assertion Tests--------------------");

		if (Asserts::EnableCoreAssertions) {
			FE_LOG_CORE_INFO("Skipping core assertion testing due to core assertion break enabled.");
		}
		else {
			FE_CORE_ASSERT(false, "core test assert 1");
			int a = 6;
			FE_CORE_ASSERT(false, "core test assert 2, val={0}", a);
		}

		if (Asserts::EnableAssertions) {
			FE_LOG_CORE_INFO("Skipping assertion testing due to assertion break enabled.");
		}
		else {
			FE_ASSERT(false, "test assert 1");
			int a = 6;
			FE_ASSERT(false, "test assert 2, val={0}", a);
		}
	}

	void Tester::Test()
	{
		FE_LOG_CORE_DEBUG("////////////////////////////////////////////////////////");
		FE_LOG_CORE_DEBUG("////////////////////// UNIT TESTS //////////////////////");
		FE_LOG_CORE_DEBUG("////////////////////////////////////////////////////////");

		LoggingTests();
		FE_LOG_CORE_DEBUG("////////////////////////////////////////////////////////");

		ProfilingTests();
		FE_LOG_CORE_DEBUG("////////////////////////////////////////////////////////");

		//EventsTests();
		//FE_LOG_CORE_DEBUG("////////////////////////////////////////////////////////");
		
		//EventsDispachersTests();
		//FE_LOG_CORE_DEBUG("////////////////////////////////////////////////////////");

		AssertionTests();
		FE_LOG_CORE_DEBUG("////////////////////////////////////////////////////////");
	}
}

#endif // FE_INTERNAL_BUILD