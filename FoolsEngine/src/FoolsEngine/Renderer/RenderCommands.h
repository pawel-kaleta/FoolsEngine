#pragma once

#include "FoolsEngine\Renderer\APIAbstraction\RendererAPI.h"

namespace fe
{
	class RenderCommands
	{
	public:
		enum class APItype
		{
			none = 0,
			OpenGL = 1
		};

		inline static APItype GetAPItype() { return s_APItype; }

		static Scope<RendererAPI> CreateAPI(APItype API);
		static void SetAPI(RendererAPI* rendererAPI, APItype API);
		inline static void InitAPI() { s_RendererAPI->Init(); }

		inline static void SetClearColor(const glm::vec4& color) { s_RendererAPI->SetClearColor(color); }

		inline static void Clear() { s_RendererAPI->Clear(); }

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray) { s_RendererAPI->DrawIndexed(vertexArray); }

	private:
		static RendererAPI* s_RendererAPI;
		static APItype s_APItype;
	};

}