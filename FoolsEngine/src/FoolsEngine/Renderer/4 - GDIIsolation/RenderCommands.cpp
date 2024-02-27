#include "FE_pch.h"
#include "RenderCommands.h"

#include "FoolsEngine\Renderer\2 - GDIAbstraction\OpenGL\OpenGLDeviceAPI.h"

namespace fe
{
	DeviceAPI* RenderCommands::s_DeviceAPI = nullptr;

	Scope<DeviceAPI> RenderCommands::CreateAPI(GDIType GDI)
	{
		switch (GDI)
		{
		case GDIType::none:
			FE_ASSERT(false, "Cannot create DeviceAPI for GDIType::none!");
			return nullptr;
		case GDIType::OpenGL:
			return CreateScope<OpenGLDeviceAPI>();
		default:
			FE_ASSERT(false, "Unknown GDIType!");
			return nullptr;
		}
	}

	void RenderCommands::SetAPI(DeviceAPI* rendererAPI)
	{
		s_DeviceAPI = rendererAPI;
	}
}