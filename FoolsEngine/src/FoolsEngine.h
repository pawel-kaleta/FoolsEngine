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
#include "FoolsEngine\Renderer\1 - Primitives\FramebufferData.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"
#include "FoolsEngine\Core\Time.h"
#include "FoolsEngine\Scene\ECS.h"
#include "FoolsEngine\Scene\Scene.h"
#include "FoolsEngine\Scene\Component.h"
#include "FoolsEngine\Scene\ComponentTypesRegistry.h"
#include "FoolsEngine\Scene\GameplayWorld\Entity.h"
#include "FoolsEngine\Scene\GameplayWorld\Actor\Actor.h"
#include "FoolsEngine\Scene\GameplayWorld\Hierarchy\HierarchyDirector.h"
#include "FoolsEngine\Scene\GameplayWorld\Hierarchy\TagsHandle.h"
#include "FoolsEngine\Scene\GameplayWorld\Hierarchy\TransformHandle.h"
#include "FoolsEngine\Scene\GameplayWorld\Hierarchy\ChildrenList.h"
#include "FoolsEngine\Scene\GameplayWorld\Actor\Behavior.h"
#include "FoolsEngine\Scene\GameplayWorld\Actor\BehaviorsRegistry.h"
#include "FoolsEngine\Scene\GameplayWorld\Actor\ActorData.h"
#include "FoolsEngine\Scene\GameplayWorld\System\SystemsRegistry.h"
#include "FoolsEngine\Assets\Serializers\SceneSerializer.h"
#include "FoolsEngine\Platform\FileDialogs.h"
#include "FoolsEngine\Math\Math.h"
#include "FoolsEngine\Assets\Loaders\TextureLoader.h"

#ifdef FE_INTERNAL_BUILD
#include "FoolsEngine\Debug\Profiler.h"
#endif // INTERNAL_BUILD

#include <yaml-cpp\yaml.h>

//---------Entry Point---------
//#include "FoolsEngine\Core\EntryPoint.h"
//-----------------------------