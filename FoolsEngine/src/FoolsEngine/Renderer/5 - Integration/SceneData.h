#pragma once

#include "FoolsEngine/Assets/AssetHandle.h"

#include "FoolsEngine/Foundation/Math/Transform.h"

#include "FoolsEngine/Renderer/1 - GAPI/Context.h"
#include "FoolsEngine/Renderer/2 - Representation/Lights.h"


namespace fe::Render::Representation { class Camera; }

namespace fe::Render::Integration
{
	struct SceneDataStruct
	{
		glm::mat4 VPMatrix;
		Representation::DirectionalLight* MainLight;
		glm::vec3 AmbientLight;
		float AmbientLightIntensity;
		const Representation::Camera* MainCamera;
		Transform CameraTransform;
		AssetID Scene;
		U32 sStreamOffset;
	};

	struct alignas(16) SceneDataGPU
	{
		GAPI::std140_mat4x4 VPMatrix;
		Representation::DirectionalLight::GPUStruct MainLight;
		GAPI::std140_vec4 AmbientLight;
		GAPI::std140_vec3 CameraPosition;
	};
}