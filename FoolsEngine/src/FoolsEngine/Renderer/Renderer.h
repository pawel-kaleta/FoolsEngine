#pragma once
namespace fe
{

	class Renderer
	{
	public:
		enum class RendererAPI
		{
			none = 0,
			OpenGL = 1
		};

		static RendererAPI GetAPI() { return s_API; }

	private:
		static RendererAPI s_API;
	};
}