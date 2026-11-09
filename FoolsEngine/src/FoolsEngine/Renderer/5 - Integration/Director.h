#pragma once

#include "FoolsEngine/Foundation/Math/Transform.h"

#include "FoolsEngine/Assets/AssetHandle.h"

#include "FoolsEngine/Renderer/1 - GAPI/Context.h"
#include "FoolsEngine/Renderer/2 - Representation/Mesh.h"
#include "FoolsEngine/Renderer/2 - Representation/Shader.h"
#include "FoolsEngine/Renderer/2 - Representation/Lights.h"
#include "FoolsEngine/Renderer/2 - Representation/Texture.h"
#include "FoolsEngine/Renderer/2 - Representation/Material.h"
#include "FoolsEngine/Renderer/2 - Representation/RenderMesh.h"
#include "FoolsEngine/Renderer/2 - Representation/ShadingModel.h"
#include "FoolsEngine/Renderer/3 - Allocator/TextureAllocator.h"

#include "BaseAssets.h"
#include "SceneData.h"


namespace fe::Render::Representation { class Camera; }

namespace fe::Render::Integration
{
	extern BaseAssetsStruct	*	BaseAssets;
	extern SceneDataStruct	*	SceneData;

	extern TextureAllocator sTextureAllocator;
	extern GAPI::GID sTextureDownStream;
	extern GAPI::GID sSceneDataStream;

	namespace Director
	{
		void Startup();
		void AcquireBaseAssets();
		void Shutdown();

		void OnWindowResize(uint32_t width, uint32_t height);

		void RenderScene(const AssetObserver& scene, const Representation::Camera& camera, const Transform& cameraTransform, GAPI::GID framebuffer);

		void BeginScene(const glm::mat4& projection, const glm::mat4& view);
		void EndScene();
	};
}