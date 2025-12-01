#include "FE_pch.h"
#include "RenderCommands.h"

#include "FoolsEngine\Renderer\1 - Description\GAPIType.h"
#include "FoolsEngine\Renderer\2 - GAPIAbstraction\OpenGL\OpenGLDeviceAPI.h"

namespace fe
{
	DeviceAPI* RenderCommands::s_DeviceAPI = nullptr;

	Scope<DeviceAPI> RenderCommands::CreateAPI(GAPIType GAPI)
	{
		switch (GAPI.Value)
		{
		case GAPIType::None:
			FE_ASSERT(false, "Cannot create DeviceAPI for GAPIType::none!");
			return nullptr;
		case GAPIType::OpenGL:
			return CreateScope<OpenGLDeviceAPI>();
		default:
			FE_ASSERT(false, "Unknown GAPIType!");
			return nullptr;
		}
	}

	void RenderCommands::SetAPI(DeviceAPI* rendererAPI)
	{
		s_DeviceAPI = rendererAPI;
	}
}