#pragma once


// To be included by FoolsEngine application (game)



#include "FoolsEngine\Debug\Log.h"
#include "FoolsEngine\Debug\Asserts.h"
#include "FoolsEngine\Core\Application.h"
#include "FoolsEngine\Events\Event.h"
#include "FoolsEngine\Core\Layer.h"
#include "FoolsEngine\ImGui\ImGuiLayer.h"
#include "FoolsEngine\Core\InputCodes.h"
#include "FoolsEngine\Core\InputPolling.h"
#include "FoolsEngine\Renderer\Renderer.h"
#include "FoolsEngine\Core\Time.h"
#include "FoolsEngine\Renderer\OrthographicCameraController.h"


#ifdef FE_INTERNAL_BUILD
#include "FoolsEngine\Debug\Profiler.h"
#include "FoolsEngine\Debug\UnitTests.h"
#endif // INTERNAL_BUILD


//---------Entry Point---------
//#include "FoolsEngine\Core\EntryPoint.h"
//-----------------------------