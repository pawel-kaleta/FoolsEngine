#pragma once

#include<glm\glm.hpp>

#include "VertexArray.h"
#include "Shader.h"

namespace fe
{

	class RendererAPI
	{
	public:
		enum class NativeAPI
		{
			none = 0,
			OpenGL = 1
		};

		inline static NativeAPI GetNativeAPI() { return s_NativeAPI; }

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) = 0;

	protected:
		static NativeAPI s_NativeAPI;
	};
}
