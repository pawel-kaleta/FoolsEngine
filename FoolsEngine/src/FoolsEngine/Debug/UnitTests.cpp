#ifdef FE_INTERNAL_BUILD

#pragma once

#include "FoolsEngine/Core.h"
#include "FoolsEngine/Debug/Log.h"
#include "FoolsEngine/Debug/UnitTests.h"
#include "FoolsEngine/Debug/Profiler.h"
#include <string>
#include <functional>

namespace fe
{
	void tester::LoggingTests()
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
		FE_LOG_CORE_DEBUG("----------------------Logging test----------------------");
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

	
	void TestEventHandler(std::shared_ptr<Event> event)
	{
		FE_LOG_CORE_DEBUG(event->ToString());
	}
	*/
	/*
	void EventsTests()
	{
		FE_LOG_CORE_DEBUG("----------------------Events tests----------------------");

		std::shared_ptr<TestEvent> event = std::make_shared<TestEvent>(99);

		if ( "Custom" == (event->GetName()) )
			FE_LOG_CORE_INFO("Event->GetName() test: PASSED" );
		else
			FE_LOG_CORE_ERROR("Event->GetName() test: FAILED");

		if (event->GetEventType() == EventType::Custom)
			FE_LOG_CORE_INFO("Event->GetEventType() test: PASSED");
		else
			FE_LOG_CORE_ERROR("Event->GetEventType() test: FAILED");

		if (event->GetStaticType() == EventType::Custom)
			FE_LOG_CORE_INFO("Event->GetStaticType() test: PASSED");
		else
			FE_LOG_CORE_ERROR("Event->GetStaticType() test: FAILED");

		if (event->GetCategoryFlags() == EventCategory::Custom)
			FE_LOG_CORE_INFO("Event->GetCategoryFlags() test: PASSED");
		else
			FE_LOG_CORE_ERROR("Event->GetCategoryFlags() test: FAILED");

		if (event->GetStaticCategoryFlags() == EventCategory::Custom)
			FE_LOG_CORE_INFO("Event->GetStaticCategoryFlags() test: PASSED");
		else
			FE_LOG_CORE_ERROR("Event->GetStaticCategoryFlags() test: FAILED");

		if (event->m_TestData == 99)
			FE_LOG_CORE_INFO("Event->m_TestData test: PASSED");
		else
			FE_LOG_CORE_ERROR("Event->m_TestData test: FAILED");

		if (event->IsInCategory(EventCategory::EngineSys))
			FE_LOG_CORE_ERROR("Event->IsInCategory() test_1: FAILED");
		else
			FE_LOG_CORE_INFO("Event->IsInCategory() test_1: PASSED");

		if (event->IsInCategory(EventCategory::Custom))
			FE_LOG_CORE_INFO("Event->IsInCategory() test_2: PASSED");
		else
			FE_LOG_CORE_ERROR("Event->IsInCategory() test_2: FAILED");

		FE_LOG_CORE_DEBUG("----------------------Events tests----------------------");
	}

	bool funtionRunned = false;
	void TestEventHandler(std::shared_ptr<Event> event)
	{
		funtionRunned = true;
	}

	void EventsDispachersTests()
	{
		FE_LOG_CORE_DEBUG("-----------------Events Dispachers tests----------------");

		std::shared_ptr<TestEvent> event = std::make_shared<TestEvent>(99);
		std::function< void(std::shared_ptr<Event>) > f = TestEventHandler;

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

	void tester::ProfilingTests()
	{
		FE_PROFILER_START_SESSION("TEST", "Logs/profiler.json");
		FE_LOG_CORE_INFO("Profiler Start Session: PASSED");
		FE_PROFILER_SCOPE_BEGIN("void ProfilingTests()");
		FE_LOG_CORE_INFO("Profiler Scope Begin: PASSED");
		{
			FE_PROFILER_SCOPE_BEGIN("Scope_lvl_1()");
			for (int i = 0; i < 100000; i++) {}
			{
				FE_PROFILER_SCOPE_BEGIN("Scope_lvl_2()");
				for (int i = 0; i < 100000; i++) {}
				{
					FE_PROFILER_SCOPE_BEGIN("Scope_lvl_3()");
					for (int i = 0; i < 100000; i++) {}
				}
				FE_LOG_CORE_INFO("Profiler Timer Stop: PASSED");
				for (int i = 0; i < 100000; i++) {}
			}
			for (int i = 0; i < 100000; i++) {}
			{
				FE_PROFILER_SCOPE_BEGIN("Scope_lvl_2()");
				for (int i = 0; i < 100000; i++) {}
				{
					FE_PROFILER_SCOPE_BEGIN("Scope_lvl_3()");
					for (int i = 0; i < 100000; i++) {}
				}
				for (int i = 0; i < 100000; i++) {}
			}
			for (int i = 0; i < 100000; i++) {}
			{
				FE_PROFILER_SCOPE_BEGIN("Scope_lvl_2()");
				for (int i = 0; i < 100000; i++) {}
				{
					FE_PROFILER_SCOPE_BEGIN("Scope_lvl_3()");
					for (int i = 0; i < 100000; i++) {}
				}
				for (int i = 0; i < 100000; i++) {}
			}
			for (int i = 0; i < 100000; i++) {}
		}
		for (int i = 0; i < 100000; i++) {}
		FE_PROFILER_END_SESSION();
		FE_LOG_CORE_INFO("Profiler End Session: PASSED");
		FE_LOG_CORE_INFO("Profiler: PASSED");
	}

	void tester::Test()
	{
		FE_LOG_CORE_DEBUG("-----------------------UNIT TESTS-----------------------");
		FE_LOG_CORE_DEBUG("--------------------------------------------------------");
		LoggingTests();
		FE_LOG_CORE_DEBUG("--------------------------------------------------------");
		FE_LOG_CORE_DEBUG("---------------------PROFILER TESTS---------------------");
		FE_LOG_CORE_DEBUG("--------------------------------------------------------");
		ProfilingTests();
		FE_LOG_CORE_DEBUG("--------------------------------------------------------");
		//EventsTests();
		//FE_LOG_CORE_DEBUG("--------------------------------------------------------");
		//EventsDispachersTests();
	}
}

#endif // FE_INTERNAL_BUILD