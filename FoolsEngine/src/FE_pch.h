#pragma once

#include <iostream>
#include <fstream>

#include <memory>
//#include <utility>
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

#include "FoolsEngine/Debug/Log.h"
#include "FoolsEngine/Debug/Asserts.h"
#include "FoolsEngine/Debug/Profiler.h"
#include "FoolsEngine/Core/InputCodes.h"
#include "FoolsEngine/Core/Core.h"