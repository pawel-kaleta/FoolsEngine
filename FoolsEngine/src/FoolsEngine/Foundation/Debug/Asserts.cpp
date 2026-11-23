#include "FoolsEngine/Foundation/Context/Context.h"

namespace fe
{
	bool EnableAssertionBreak = true;
	bool EnableCoreAssertionBreak = true;

	void HandleAssert(const std::source_location& location, const char* message)
	{
		Context->Log.FE.Error("CORE ASSERTION FAILED:"); 
		//Context->Log.FE.mLogger->error(spdlog::fmt_lib::runtime("File:     {}"), std::filesystem::path(__FILE__).filename().string());
		Context->Log.FE.mLogger->error(spdlog::fmt_lib::runtime("File:     {}"), location.file_name());
		Context->Log.FE.mLogger->error(spdlog::fmt_lib::runtime("Function: {}"), location.function_name());
		Context->Log.FE.mLogger->error(spdlog::fmt_lib::runtime("Line:     {}"), location.line());
		Context->Log.FE.mLogger->error(spdlog::fmt_lib::runtime("Message:  {}"), message);
	}
}