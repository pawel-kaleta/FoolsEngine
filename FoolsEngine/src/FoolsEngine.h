#pragma once



// To be included by FoolsEngine application (game)

#include "FoolsEngine/Debug/Log.h"
#include "FoolsEngine/Application.h"
#include "FoolsEngine/Events/Events.h"


#ifdef FE_INTERNAL_BUILD
#include "FoolsEngine/Debug/Profiler.h"

#include "FoolsEngine/Debug/UnitTests.h"
#endif // INTERNAL_BUILD


//-----Entry Point-------------
#include "FoolsEngine/EntryPoint.h"
//-----------------------------