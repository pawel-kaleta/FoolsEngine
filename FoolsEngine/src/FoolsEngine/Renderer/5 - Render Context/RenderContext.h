#pragma once

#include "CommandQue.h"
#include "PipelineState.h"
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

		ResourceManager ResourceManager;
		PipelineState PipelineState;
	};

	struct OpenGLRenderContext final : public RenderContext
	{
		GLFWwindow* BaseWindowInvisible = nullptr;

		bool Create();
	};
}