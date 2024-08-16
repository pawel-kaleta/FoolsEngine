#pragma once


#include <iostream>
#include <fstream>

#include <memory>
#include <algorithm>
#include <functional>
#include <iomanip>
#include <chrono>
#include <thread>
#include <mutex>

#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>

#ifdef FE_PLATFORM_WINDOWS
	#define NOMINMAX
	// max() macro collides with entt library
	#include <Windows.h>
#endif

#include "FoolsEngine/Debug/Log.h"
#include "FoolsEngine/Debug/Asserts.h"
#include "FoolsEngine/Debug/Profiler.h"
#include "FoolsEngine/Platform/InputCodes.h"
#include "FoolsEngine/Core/Core.h"