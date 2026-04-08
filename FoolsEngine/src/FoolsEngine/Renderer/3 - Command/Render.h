#pragma once

#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"
#include "FoolsEngine/Renderer/2 - Resource/RTexture.h"
#include "FoolsEngine/Renderer/2 - Resource/RMeshBindings.h"

namespace fe::Command
{
	namespace Render
	{
		template <GAPIType::ValueType GAPI> void DrawIndexed(const Resource::RMeshBindings<GAPI>& meshBindings);

		//Vulkan
		//template <> void DrawIndexed<GAPIType::Vulkan>(const Resource::RMeshBindings<GAPIType::Vulkan>& meshBindings) {}
		
		//OpenGL
		template <> void DrawIndexed<GAPIType::OpenGL>(	const Resource::RMeshBindings<GAPIType::OpenGL>	& meshBindings);
		inline		void DrawIndexed_OpenGL(			const Resource::RMeshBindings_OpenGL			& meshBindings) { DrawIndexed<GAPIType::OpenGL>(meshBindings); }
	}
}