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

#include "FoolsEngine/Foundation/Debug/Log.h"
#include "FoolsEngine/Foundation/Debug/Asserts.h"
#include "FoolsEngine/Foundation/Debug/Profiler.h"
#include "FoolsEngine/Foundation/Utils/Core.h"
#include "FoolsEngine/Foundation/Utils/DeclareEnum.h"
#include "FoolsEngine/Foundation/Utils/ForEach.h"
#include "FoolsEngine/Foundation/Memory/Scratchpad.h"

#include "FoolsEngine/Platform/InputCodes.h"