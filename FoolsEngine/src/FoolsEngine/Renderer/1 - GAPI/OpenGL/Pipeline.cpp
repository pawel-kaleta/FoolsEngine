#include "FoolsEngine/Foundation/DataTypes.h"

#include "FoolsEngine/Renderer/1 - GAPI/Pipeline.h"

#include "GraphicsPipeline.h"
#include "Shader.h"
#include "Texture.h"
#include "DownStream.h"
#include "Buffer.h"
#include "Registry.h"


namespace fe::Render::GAPI
{
	GID CreateGraphicsPipelineCmd(GID vertexShader, GID fragmentShader, const Descriptors::Raster::Specification& spec)
	{
		FE_PROFILER_FUNC();

		OpenGL::InternalID id = OpenGL::GraphicsPipelineRegistry.GetNewID();
		OpenGL::GraphicsPipeline* obj = OpenGL::GraphicsPipelineRegistry.GetObj(id);

		OpenGL::Shader* vert_shader = OpenGL::ShaderRegistry.GetObj(vertexShader);
		OpenGL::Shader* fragment_shader = OpenGL::ShaderRegistry.GetObj(fragmentShader);

		obj->Init();
		obj->CreateCmd(*vert_shader, *fragment_shader, spec);
		
		return id;
	}

	void DestroyPipelineCmd(GID pipeline)
	{
		FE_PROFILER_FUNC();

		OpenGL::GraphicsPipeline* pipeline_obj = OpenGL::GraphicsPipelineRegistry.GetObj(pipeline);
		pipeline_obj->DestroyCmd();
		OpenGL::GraphicsPipelineRegistry.FreeObj(pipeline);
	}
}