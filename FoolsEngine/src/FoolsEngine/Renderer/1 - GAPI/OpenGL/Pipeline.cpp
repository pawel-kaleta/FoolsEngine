#pragma once

#include "FE_pch.h"

#include "FoolsEngine/Foundation/Memory/DataTypes.h"

#include "FoolsEngine/Renderer/1 - GAPI/Pipeline.h"

#include "GraphicsPipeline.h"
#include "Shader.h"
#include "Texture.h"
#include "DownStream.h"
#include "Buffer.h"

namespace fe::GAPI::Pipeline
{
	GID CreateGraphicsPipelineCmd(GID vertexShader, GID fragmentShader, const Raster::Specification& spec)
	{

	}

	void SetColorAttachment(GID pipeline, GID texture, UInt index)
	{

	}

	void SetDepthStencilAttachment(GID pipeline, GID texture)
	{

	}

	void SetDepthStencilSpec(GID pipeline, const DepthStencil::Specification& depthStencil)
	{

	}

	void SetBlendSpec(GID pipeline, const Blend::Specification& blend)
	{

	}

	void SetUBO(GID pipeline, GID stream, UInt bindingIndex)
	{

	}

	void SetSSBO(GID pipeline, GID buffer, UInt bindingIndex)
	{

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