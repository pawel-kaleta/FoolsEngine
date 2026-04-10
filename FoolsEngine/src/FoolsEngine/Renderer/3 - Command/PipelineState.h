#pragma once

#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"
#include "FoolsEngine/Renderer/1 - Description/Pipeline.h"
#include "FoolsEngine/Renderer/2 - Resource/RTexture.h"
#include "FoolsEngine/Renderer/2 - Resource/RMeshBindings.h"
#include "FoolsEngine/Renderer/2 - Resource/RFrameBuffer.h"

namespace fe::Command
{
	struct PipelineState
	{
		virtual void BindFramebuffer(const Resource::RFramebuffer& framebuffer) = 0;
		virtual void BindMeshBindings(const Resource::RMeshBindings& meshBindings) = 0;
		virtual void BindTextureToRendererTextureSlot(U32 rendererTextureSlot, const Resource::RTexture& texture) = 0;
		virtual void SetDepthTest(bool enable) = 0;
		virtual void SetDepthTestType(Description::Pipeline::DepthTestType type) = 0;
		virtual void SetBlendFunction(Description::Pipeline::BlendFunction source, Description::Pipeline::BlendFunction destination) = 0;
		virtual void SetBlending(bool enable) = 0;
		virtual void SetViewport(U32 x, U32 y, U32 width, U32 height) = 0;
	};

	struct PipelineState_OpenGL final : public PipelineState
	{
		virtual void BindFramebuffer(const Resource::RFramebuffer& framebuffer) final override;
		virtual void BindMeshBindings(const Resource::RMeshBindings& meshBindings) final override;
		virtual void BindTextureToRendererTextureSlot(U32 rendererTextureSlot, const Resource::RTexture& texture) final override;
		virtual void SetDepthTest(bool enable) final override;
		virtual void SetDepthTestType(Description::Pipeline::DepthTestType type) final override;
		virtual void SetBlendFunction(Description::Pipeline::BlendFunction source, Description::Pipeline::BlendFunction destination) final override;
		virtual void SetBlending(bool enable) final override;
		virtual void SetViewport(U32 x, U32 y, U32 width, U32 height) final override;
	};
}