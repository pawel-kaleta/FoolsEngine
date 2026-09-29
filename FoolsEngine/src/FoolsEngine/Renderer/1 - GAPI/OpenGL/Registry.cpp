#pragma once

#include "FE_pch.h"

#include "Registry.h"

namespace fe::Render::GAPI::OpenGL
{
	Registry<GraphicsPipeline> GraphicsPipelineRegistry;
	Registry<Buffer> BufferRegistry;
	Registry<Texture> TextureRegistry;
	Registry<TextureView> TextureViewRegistry;
	Registry<Shader> ShaderRegistry;
	Registry<Region> RegionRegistry;
	Registry<Region> RegionRegistry;
	Registry<DownStream> DownStreamRegistry;

	void CreateRegistries()
	{
		GraphicsPipelineRegistry.Create();
		BufferRegistry.Create();
		TextureRegistry.Create();
		TextureViewRegistry.Create();
		ShaderRegistry.Create();
		RegionRegistry.Create();
		RegionRegistry.Create();
		DownStreamRegistry.Create();
	}
}