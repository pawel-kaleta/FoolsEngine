#pragma once

#include "Texture.h"
#include "Buffer.h"
#include "ShaderInterface.h"
#include "Framebuffer.h"

#include <vector>
#include <memory_resource>

namespace fe::Description
{
	struct Library
	{
		std::pmr::monotonic_buffer_resource m_Allocator;

		std::pmr::vector<Texture::Specification> TextureSpecs;
		std::pmr::vector<Buffer::Layout> BufferLayouts;
		std::pmr::vector<ShaderInterface::Specification> ShaderSpecs;
		std::pmr::vector<ShaderInterface::ProgramSpecification> ProgramSpecs;
		std::pmr::vector<Framebuffer::Specification> FramebufferSpecs;
		
		static Library& Get() { return *s_Library; }
	private:
		Library() :
			m_Allocator(),
			TextureSpecs(&m_Allocator),
			BufferLayouts(&m_Allocator),
			ShaderSpecs(&m_Allocator),
			ProgramSpecs(&m_Allocator),
			FramebufferSpecs(&m_Allocator)
		{ }

		friend class Application;
		static void Shutdown() { }
		static void Init()
		{
			s_Library = new Library();
		}

		static Library* s_Library;
	};
}