#pragma once

#include "CommandQue.h"
#include "PipelineManager.h"
#include "ProgramManager.h"
#include "ResourceManager.h"
#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace fe
{
	struct RenderContextID
	{
		U32 Value = -1;
		void Create()
		{
			static U32 counter = 0;

			Value = counter;
			counter++;
		}
	};

	struct RenderContext
	{
		RenderContextID ID;
		GAPIType GAPIType;

	};

	struct RenderContext_OpenGL final : public RenderContext
	{
		GLFWwindow* BaseWindow = nullptr;
		ResourceManager_OpenGL ResourceManager;
		PipelineManager_OpenGL PipelineState;


		bool Create();
	};
}