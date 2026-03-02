#pragma once

// To be included by FoolsEngine application (game)
// To do: create a nice include folder with include batches based on areas/namespaces

#include "FoolsEngine\Debug\Log.h"
#include "FoolsEngine\Debug\Asserts.h"
#include "FoolsEngine\Core\Application.h"
#include "FoolsEngine\Events\Event.h"
#include "FoolsEngine\Core\Layer.h"
#include "FoolsEngine\Core\Project.h"
#include "FoolsEngine\ImGui\ImGuiLayer.h"
#include "FoolsEngine\Platform\InputCodes.h"
#include "FoolsEngine\Platform\InputPolling.h"
#include "FoolsEngine\Renderer\1 - Description\Framebuffer.h"
#include "FoolsEngine\Renderer\2 - Resource\Framebuffer.h"
#include "FoolsEngine\Renderer\3 - Command\PipelineState.h"
#include "FoolsEngine\Renderer\3 - Command\ResourceState.h"
#include "FoolsEngine\Renderer\7 - Integration\Renderer.h"
#include "FoolsEngine\Core\Time.h"
#include "FoolsEngine\Scene\ECS.h"
#include "FoolsEngine\Scene\Scene.h"
#include "FoolsEngine\Scene\Component.h"
#include "FoolsEngine\Scene\ComponentTypesRegistry.h"
#include "FoolsEngine\Scene\Components\RenderingComponents.h"
#include "FoolsEngine\Scene\Components\2DComponents.h"
#include "FoolsEngine\Scene\Components\MeshComponents.h"
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
#include "FoolsEngine\Assets\Serialization\SceneSerializer.h"
#include "FoolsEngine\Platform\FileDialogs.h"
#include "FoolsEngine\Math\Math.h"
#include "FoolsEngine\Assets\Loaders\LoaderType.h"
#include "FoolsEngine\Assets\Loaders\TextureLoader.h"
#include "FoolsEngine\Assets\Loaders\GeometryLoader.h"
#include "FoolsEngine\Assets\Loaders\ShaderLoader.h"
#include "FoolsEngine\Assets\Loaders\LoadersRegistry.h"
#include "FoolsEngine\Assets\Serialization\YAML.h"
#include "FoolsEngine\Assets\Serialization\AssetsRegistrySerialization.h"
#include "FoolsEngine\Assets\AssetManager.h"
#include "FoolsEngine\Assets\AssetTypesRegistry.h"
#include "FoolsEngine\Memory\Scratchpad.h"
#include "FoolsEngine\Utils\Colors.h"
#include "FoolsEngine\Utils\BitOperations.h"
#include "FoolsEngine\Utils\Xar.h"

#ifdef FE_INTERNAL_BUILD
#include "FoolsEngine\Debug\Profiler.h"
#endif // INTERNAL_BUILD

#include <yaml-cpp\yaml.h>

#include <glm\gtc\type_ptr.hpp>

#include <assimp\scene.h>

//---------Entry Point---------
//#include "FoolsEngine\Core\EntryPoint.h"
//-----------------------------


// TO DO:
// - standarize asset loading/unloading to CPU/GPU ("sendtogpu(), Release(), unloadfromcpu(), releasedependencyload() etc.)
// - cleanup of rendering architecture (acces to scene data)
// - asset import is most likely totally broken after low-lever renderer redesing (descriptors global allocation)
// - what about unused descriptors left after removed assets?