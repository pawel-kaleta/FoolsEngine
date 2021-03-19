#pragma once

#include <iostream>
#include <fstream>

#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <iomanip>
#include <chrono>
#include <thread>
#include <mutex>

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#ifdef FE_PLATFORM_WINDOWS
	#include <Windows.h>
#endif

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/fmt/ostr.h"

#include "FoolsEngine/Debug/Log.h"
#include "FoolsEngine/Debug/Asserts.h"
#include "FoolsEngine/Debug/Profiler.h"
