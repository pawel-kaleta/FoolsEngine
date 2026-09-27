#pragma once

#include "FE_pch.h"

#include "FoolsEngine/Foundation/Memory/DataTypes.h"

#include "FoolsEngine/Renderer/1 - GAPI/Pipeline.h"

#include "GraphicsPipeline.h"
#include "Shader.h"
#include "Texture.h"
#include "DownStream.h"
#include "Buffer.h"
#include "Registry.h"


namespace fe::GAPI::Pipeline
{
	GID CreateGraphicsPipelineCmd(GID vertexShader, GID fragmentShader, const Raster::Specification& spec)
	{
		OpenGL::InternalID id = OpenGL::GraphicsPipelineRegistry.GetNewID();
		OpenGL::GraphicsPipeline* obj = OpenGL::GraphicsPipelineRegistry.GetObj(id);

		OpenGL::Shader* vert_shader = OpenGL::ShaderRegistry.GetObj(vertexShader);
		OpenGL::Shader* fragment_shader = OpenGL::ShaderRegistry.GetObj(fragmentShader);

		obj->Init();
		obj->CreateCmd(*vert_shader, *fragment_shader, spec);
		
		return id;
	}

	void SetColorAttachment(GID pipeline, GID texture, UInt index)
	{
		OpenGL::GraphicsPipeline* pipeline_obj = OpenGL::GraphicsPipelineRegistry.GetObj(pipeline);
		OpenGL::Texture* texture_obj = OpenGL::TextureRegistry.GetObj(texture);
		pipeline_obj->SetColorAttachment(*texture_obj, index);
	}

	void SetDepthStencilAttachment(GID pipeline, GID texture)
	{
		OpenGL::GraphicsPipeline* pipeline_obj = OpenGL::GraphicsPipelineRegistry.GetObj(pipeline);
		OpenGL::Texture* texture_obj = OpenGL::TextureRegistry.GetObj(texture);
		pipeline_obj->SetDepthStencilAttachment(*texture_obj);
	}

	void SetDepthStencilSpec(GID pipeline, const DepthStencil::Specification& depthStencil)
	{
		OpenGL::GraphicsPipeline* pipeline_obj = OpenGL::GraphicsPipelineRegistry.GetObj(pipeline);
		pipeline_obj->SetDepthStencilSpec(depthStencil);
	}

	void SetBlendSpec(GID pipeline, const Blend::Specification& blend)
	{
		OpenGL::GraphicsPipeline* pipeline_obj = OpenGL::GraphicsPipelineRegistry.GetObj(pipeline);
		pipeline_obj->SetBlendSpec(blend);
	}

	void SetUBO(GID pipeline, GID stream, UInt bindingIndex)
	{
		OpenGL::GraphicsPipeline* pipeline_obj = OpenGL::GraphicsPipelineRegistry.GetObj(pipeline);
		OpenGL::Stream* stream_obj = OpenGL::DownStreamRegistry.GetObj(stream); // to do: test stream type and fail if upstream
		pipeline_obj->SetUniformBuffer(stream_obj, bindingIndex);
	}

	void SetSSBO(GID pipeline, GID buffer, UInt bindingIndex)
	{
		OpenGL::GraphicsPipeline* pipeline_obj = OpenGL::GraphicsPipelineRegistry.GetObj(pipeline);
		OpenGL::Buffer* buffer_obj = OpenGL::BufferRegistry.GetObj(buffer);
		pipeline_obj->SetShaderStorageBuffer(buffer_obj, bindingIndex);
	}

	// buffer or stream
	void SetIndexSource(GID pipeline, GID source)
	{

	}

	// buffer or stream
	void SetDrawParamsSource(GID pipeline, GID source)
	{

	}

	void ActivatePipelineCmd(GID pipeline)
	{

	}

	void DrawCmd(UInt primitiveCount, UInt indicesOffset)
	{

	}

	void DrawIndirectCmd(UInt paramsOffset)
	{

	}

	void MultiDrawIndirectCmd(UInt paramsOffset, UInt drawCount)
	{

	}

	void MultiDrawIndirectCountCmd(UInt paramsOffset, UInt maxDrawCount)
	{

	}

	void DeactivatePipelineCmd()
	{

	}

	void DestroyPipelineCmd(GID pipeline)
	{

	}
}