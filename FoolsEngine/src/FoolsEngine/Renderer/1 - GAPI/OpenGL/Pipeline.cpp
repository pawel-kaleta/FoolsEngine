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


namespace fe::Render::GAPI
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

	// buffer or downstream
	void SetUBO(GID pipeline, GID source, UInt bindingIndex)
	{
		OpenGL::GraphicsPipeline* pipeline_obj = OpenGL::GraphicsPipelineRegistry.GetObj(pipeline);
		GLuint buffer;

		switch ((ObjType::ValueType)((OpenGL::InternalID)source).mComps.Type)
		{
		case ObjType::Buffer:		buffer = OpenGL::BufferRegistry.GetObj(source)->mGLID; break;
		case ObjType::DownStream:	buffer = OpenGL::DownStreamRegistry.GetObj(source)->mOpenGLBuffer; break;
		case ObjType::None:
		default:
			FE_CORE_ASSERT(false, "Urecognised type of source for UBO binding");
		}

		pipeline_obj->SetUniformBuffer(buffer, bindingIndex);
	}

	// buffer, downstream or upstream
	void SetSSBO(GID pipeline, GID target, UInt bindingIndex)
	{
		OpenGL::GraphicsPipeline* pipeline_obj = OpenGL::GraphicsPipelineRegistry.GetObj(pipeline);
		GLuint buffer_gl;

		switch ((ObjType::ValueType)((OpenGL::InternalID)target).mComps.Type)
		{
		case ObjType::Buffer:		buffer_gl = OpenGL::BufferRegistry.GetObj(target)->mGLID; break;
		case ObjType::DownStream:	buffer_gl = OpenGL::DownStreamRegistry.GetObj(target)->mOpenGLBuffer; break;
		case ObjType::UpStream:		FE_CORE_ASSERT(false, "Not implemented yet"); return;
		case ObjType::None:
		default:
			FE_CORE_ASSERT(false, "Urecognised type of target for SSBO binding");
		}

		pipeline_obj->SetShaderStorageBuffer(buffer_gl, bindingIndex);
	}

	// buffer or downstream
	void SetIndexSource(GID pipeline, GID source)
	{
		OpenGL::GraphicsPipeline* pipeline_obj = OpenGL::GraphicsPipelineRegistry.GetObj(pipeline);
		GLuint buffer_gl;

		switch ((ObjType::ValueType)((OpenGL::InternalID)source).mComps.Type)
		{
		case ObjType::Buffer:		buffer_gl = OpenGL::BufferRegistry.GetObj(source)->mGLID; break;
		case ObjType::DownStream:	buffer_gl = OpenGL::DownStreamRegistry.GetObj(source)->mOpenGLBuffer; break;
		case ObjType::None:
		default:
			FE_CORE_ASSERT(false, "Urecognised type of target for SSBO binding");
		}

		pipeline_obj->SetIndicesBuffer(buffer_gl);
	}

	// buffer or downstream
	void SetDrawParamsSource(GID pipeline, GID source)
	{
		OpenGL::GraphicsPipeline* pipeline_obj = OpenGL::GraphicsPipelineRegistry.GetObj(pipeline);
		GLuint buffer_gl;

		switch ((ObjType::ValueType)((OpenGL::InternalID)source).mComps.Type)
		{
		case ObjType::Buffer:		buffer_gl = OpenGL::BufferRegistry.GetObj(source)->mGLID; break;
		case ObjType::DownStream:	buffer_gl = OpenGL::DownStreamRegistry.GetObj(source)->mOpenGLBuffer; break;
		case ObjType::None:
		default:
			FE_CORE_ASSERT(false, "Urecognised type of target for SSBO binding");
		}

		pipeline_obj->SetDrawParamsBuffer(buffer_gl);
	}

	void ActivatePipelineCmd(GID pipeline)
	{
		OpenGL::GraphicsPipeline* pipeline_obj = OpenGL::GraphicsPipelineRegistry.GetObj(pipeline);
		pipeline_obj->ActivateCmd();
	}

	void DrawCmd(GID pipeline, UInt primitiveCount, UInt indicesOffset)
	{
		OpenGL::GraphicsPipeline* pipeline_obj = OpenGL::GraphicsPipelineRegistry.GetObj(pipeline);
		pipeline_obj->DrawCmd(primitiveCount, indicesOffset);
	}

	void DrawIndirectCmd(GID pipeline, UInt paramsOffset)
	{
		OpenGL::GraphicsPipeline* pipeline_obj = OpenGL::GraphicsPipelineRegistry.GetObj(pipeline);
		pipeline_obj->DrawIndirectCmd(paramsOffset);
	}

	void MultiDrawIndirectCmd(GID pipeline, UInt paramsOffset, UInt drawCount)
	{
		OpenGL::GraphicsPipeline* pipeline_obj = OpenGL::GraphicsPipelineRegistry.GetObj(pipeline);
		pipeline_obj->MultiDrawIndirectCmd(paramsOffset, drawCount);
	}

	void MultiDrawIndirectCountCmd(GID pipeline, UInt paramsOffset, UInt maxDrawCount)
	{
		OpenGL::GraphicsPipeline* pipeline_obj = OpenGL::GraphicsPipelineRegistry.GetObj(pipeline);
		pipeline_obj->MultiDrawIndirectCountCmd(paramsOffset, maxDrawCount);
	}

	void DestroyPipelineCmd(GID pipeline)
	{
		OpenGL::GraphicsPipeline* pipeline_obj = OpenGL::GraphicsPipelineRegistry.GetObj(pipeline);
		pipeline_obj->DestroyCmd();
		OpenGL::GraphicsPipelineRegistry.FreeObj(pipeline);
	}
}