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
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"
#include "FoolsEngine\Core\Time.h"
#include "FoolsEngine\Scene\Scene.h"
#include "FoolsEngine\Scene\Component.h"
#include "FoolsEngine\Scene\Entity.h"
#include "FoolsEngine\Scene\Actor.h"
#include "FoolsEngine\Scene\Hierarchy\EntitiesHierarchy.h"
#include "FoolsEngine\Scene\Hierarchy\TagsHandle.h"
#include "FoolsEngine\Scene\Hierarchy\TransformHandle.h"
#include "FoolsEngine\Scene\Hierarchy\ChildrenList.h"
#include "FoolsEngine\Scene\Behavior.h"
#include "FoolsEngine\Scene\ActorData.h"


#ifdef FE_INTERNAL_BUILD
#include "FoolsEngine\Debug\Profiler.h"
#include "FoolsEngine\Debug\UnitTests.h"
#endif // INTERNAL_BUILD

#include <entt.hpp>

//---------Entry Point---------
//#include "FoolsEngine\Core\EntryPoint.h"
//-----------------------------