#pragma once

#include "FE_pch.h"

#include "Registry.h"

namespace fe::GAPI::OpenGL
{
	Registry<GraphicsPipeline> GraphicsPipelineRegistry;
	Registry<Buffer> BufferRegistry;
	Registry<Texture> TextureRegistry;
	Registry<TextureView> TextureViewRegistry;
	Registry<Shader> ShaderRegistry;
	Registry<Region> RegionRegistry;
	Registry<Region> RegionRegistry;
	Registry<DownStream> DownStreamRegistry;

	// to do: create these
}