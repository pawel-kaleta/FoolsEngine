#pragma once

#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"
#include "FoolsEngine/Renderer/1 - Description/Pipeline.h"
#include "FoolsEngine/Renderer/2 - Resource/RTexture.h"
#include "FoolsEngine/Renderer/2 - Resource/RMeshBindings.h"
#include "FoolsEngine/Renderer/2 - Resource/RFrameBuffer.h"

namespace fe::Command
{
	namespace PipelineState
	{
		template <GAPIType::ValueType GAPI> void BindFramebuffer(const Resource::RFramebuffer<GAPI>& framebuffer);
		template <GAPIType::ValueType GAPI> void BindMeshBindings(const Resource::RMeshBindings<GAPI>& meshBindings);
		template <GAPIType::ValueType GAPI> void BindTextureToRendererTextureSlot(U32 rendererTextureSlot, const Resource::RTexture<GAPI>& texture);
		template <GAPIType::ValueType GAPI> void SetDepthTest(bool enable);
		template <GAPIType::ValueType GAPI> void SetDepthTestType(Description::Pipeline::DepthTestType type);
		template <GAPIType::ValueType GAPI> void SetBlendFunction(Description::Pipeline::BlendFunction source, Description::Pipeline::BlendFunction destination);
		template <GAPIType::ValueType GAPI> void SetBlending(bool enable);
		template <GAPIType::ValueType GAPI> void SetViewport(U32 x, U32 y, U32 width, U32 height);


		// Vulkan
		template <> void BindFramebuffer<GAPIType::Vulkan>(const Resource::RFramebuffer<GAPIType::Vulkan>& framebuffer);

		// OpenGL
		template <> void BindFramebuffer<GAPIType::OpenGL>(	const Resource::RFramebuffer<GAPIType::OpenGL>	& framebuffer);
		inline		void BindFramebuffer_OpenGL(			const Resource::RFramebuffer_OpenGL				& framebuffer) { BindFramebuffer<GAPIType::OpenGL>(framebuffer); };
		
		template <> void BindMeshBindings<GAPIType::OpenGL>(const Resource::RMeshBindings<GAPIType::OpenGL>	& meshBindings);
		inline		void BindMeshBindings_OpenGL(			const Resource::RMeshBindings_OpenGL			& meshBindings) { BindMeshBindings<GAPIType::OpenGL>(meshBindings); };
		
		template <> void BindTextureToRendererTextureSlot<GAPIType::OpenGL>(U32 rendererTextureSlot, const Resource::RTexture<GAPIType::OpenGL>	& texture);
		inline		void BindTextureToRendererTextureSlot_OpenGL(			U32 rendererTextureSlot, const Resource::RTexture_OpenGL			& texture) { BindTextureToRendererTextureSlot<GAPIType::OpenGL>(rendererTextureSlot, texture); };

		template <> void SetDepthTest<GAPIType::OpenGL>(bool enable);
		inline		void SetDepthTest_OpenGL(bool enable) { SetDepthTest<GAPIType::OpenGL>(enable); }

		template <> void SetDepthTestType<GAPIType::OpenGL>(Description::Pipeline::DepthTestType type);
		inline		void SetDepthTestType_OpenGL(			Description::Pipeline::DepthTestType type) { SetDepthTestType<GAPIType::OpenGL>(type); }

		template <> void SetBlendFunction<GAPIType::OpenGL>(Description::Pipeline::BlendFunction source, Description::Pipeline::BlendFunction destination);
		inline		void SetBlendFunction_OpenGL(			Description::Pipeline::BlendFunction source, Description::Pipeline::BlendFunction destination) { SetBlendFunction<GAPIType::OpenGL>( source, destination); }
		
		template <> void SetBlending<GAPIType::OpenGL>(	bool enable);
		inline		void SetBlending_OpenGL(			bool enable) { SetBlending<GAPIType::OpenGL>(enable); }

		template <> void SetViewport<GAPIType::OpenGL>(	U32 x, U32 y, U32 width, U32 height);
		inline		void SetViewport_OpenGL(			U32 x, U32 y, U32 width, U32 height) { SetViewport<GAPIType::OpenGL>(x, y, width, height); }

	}
}