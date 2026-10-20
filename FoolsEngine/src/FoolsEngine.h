#pragma once

// To be included by FoolsEngine application (game)
// To do: create a nice include folder with include batches based on areas/namespaces

#include "FoolsEngine/Foundation/Math/Math.h"
#include "FoolsEngine/Foundation/Debug/Asserts.h"
#include "FoolsEngine/Foundation/Utils/Colors.h"
#include "FoolsEngine/Foundation/Utils/BitOperations.h"
#include "FoolsEngine/Foundation/Containers/Xar.h"

#ifdef FE_INTERNAL_BUILD
#include "FoolsEngine/Foundation/Debug/Profiler.h"
#endif // INTERNAL_BUILD

#include "FoolsEngine/Platform/InputCodes.h"
#include "FoolsEngine/Platform/FileDialogs.h"
#include "FoolsEngine/Platform/InputPolling.h"
#include "FoolsEngine/Platform/Events/Event.h"

#include "FoolsEngine/Application/Time.h"
#include "FoolsEngine/Application/Layer.h"
#include "FoolsEngine/Application/Project.h"
#include "FoolsEngine/Application/Application.h"
#include "FoolsEngine/Application/ImGui/ImGuiLayer.h"

#include "FoolsEngine/Scene/ECS.h"
#include "FoolsEngine/Scene/Scene.h"
#include "FoolsEngine/Scene/Component.h"
#include "FoolsEngine/Scene/ComponentTypesRegistry.h"
#include "FoolsEngine/Scene/Components/2DComponents.h"
#include "FoolsEngine/Scene/Components/MeshComponents.h"
#include "FoolsEngine/Scene/Components/RenderingComponents.h"
#include "FoolsEngine/Scene/GameplayWorld/Entity.h"
#include "FoolsEngine/Scene/GameplayWorld/Actor/Actor.h"
#include "FoolsEngine/Scene/GameplayWorld/Actor/Behavior.h"
#include "FoolsEngine/Scene/GameplayWorld/Actor/ActorData.h"
#include "FoolsEngine/Scene/GameplayWorld/Actor/BehaviorsRegistry.h"
#include "FoolsEngine/Scene/GameplayWorld/System/SystemTypesRegistry.h"
#include "FoolsEngine/Scene/GameplayWorld/Hierarchy/TagsHandle.h"
#include "FoolsEngine/Scene/GameplayWorld/Hierarchy/ChildrenList.h"
#include "FoolsEngine/Scene/GameplayWorld/Hierarchy/TransformHandle.h"
#include "FoolsEngine/Scene/GameplayWorld/Hierarchy/HierarchyDirector.h"

#include "FoolsEngine/Assets/AssetManager.h"
#include "FoolsEngine/Assets/AssetTypesRegistry.h"
#include "FoolsEngine/Assets/Loaders/LoaderType.h"
#include "FoolsEngine/Assets/Loaders/ShaderLoader.h"
#include "FoolsEngine/Assets/Loaders/TextureLoader.h"
#include "FoolsEngine/Assets/Loaders/GeometryLoader.h"
#include "FoolsEngine/Assets/Loaders/LoadersRegistry.h"
#include "FoolsEngine/Assets/Serialization/YAML.h"
#include "FoolsEngine/Assets/Serialization/SceneSerializer.h"
#include "FoolsEngine/Assets/Serialization/AssetsRegistrySerialization.h"

#include "FoolsEngine/Renderer/5 - Integration/Renderer.h"

#include <yaml-cpp/yaml.h>

#include <glm/gtc/type_ptr.hpp>

#include <assimp/scene.h>

//---------Entry Point---------
//#include "FoolsEngine/Core/EntryPoint.h"
//-----------------------------


// TO DO:
// - cleanup of rendering architecture (acces to scene data)
// - asset import is most likely totally broken after low-lever renderer redesing
// - get rid of this file and provide some dedicated domain specific includes