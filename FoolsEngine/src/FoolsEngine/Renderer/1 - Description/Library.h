#pragma once

#include "Buffer.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "ShaderInterface.h"

#include "FoolsEngine/Foundation/Common.h"
#include "FoolsEngine/Foundation/Memory/XarAlloc.h"

#include <memory_resource>

namespace fe::Description
{
	struct Library
	{
		MonotonicAlloc	m_AllocPermanent;
		MallocAlloc		m_AllocGPA;
		STD_PMR_Allocator<MallocAlloc>	m_AllocGPA_STD_PMR;

		XarrAlloc<Texture::Archetype,						MonotonicAlloc, MallocAlloc>	TextureArchetypes;
		XarrAlloc<Buffer::Layout,							MonotonicAlloc, MallocAlloc>	BufferLayouts;
		XarrAlloc<Framebuffer::Specification,				MonotonicAlloc, MallocAlloc>	FramebufferSpecs;
		XarrAlloc<ShaderInterface::TextureSampler,			MonotonicAlloc, MallocAlloc>	TextureSamplers;
		XarrAlloc<ShaderInterface::UniformBufferSampler,	MonotonicAlloc, MallocAlloc>	UniformBufferSamplers;
		XarrAlloc<ShaderInterface::Specification,			MonotonicAlloc, MallocAlloc>	ShaderSpecs;
		XarrAlloc<ShaderInterface::ProgramSpecification,	MonotonicAlloc, MallocAlloc>	ProgramSpecs;
		
		std::pmr::map<UUID, U32> UUIDToIDMap;
		static Library& Get() { return *s_Library; }

		template <typename tnDescriptor>
		UInt CreateOrGetDescriptorWithUUID(UUID uuid)
		{
			if constexpr (std::same_as<tnDescriptor, Texture::Archetype						>) return CreateOrGetDescriptorWithUUID_Texture(uuid);
			if constexpr (std::same_as<tnDescriptor, Buffer::Layout							>) return CreateOrGetDescriptorWithUUID_Layout(uuid);
			if constexpr (std::same_as<tnDescriptor, Framebuffer::Specification				>) return CreateOrGetDescriptorWithUUID_Framebuffer(uuid);
			if constexpr (std::same_as<tnDescriptor, ShaderInterface::TextureSampler		>) return CreateOrGetDescriptorWithUUID_TextureSampler(uuid);
			if constexpr (std::same_as<tnDescriptor, ShaderInterface::UniformBufferSampler	>) return CreateOrGetDescriptorWithUUID_UniformBufferSampler(uuid);
			if constexpr (std::same_as<tnDescriptor, ShaderInterface::Specification			>) return CreateOrGetDescriptorWithUUID_ShaderInterface(uuid);
			if constexpr (std::same_as<tnDescriptor, ShaderInterface::ProgramSpecification	>) return CreateOrGetDescriptorWithUUID_ProgramSpecification(uuid);
		}
		
	private:
		Library();

		friend class Application;
		static void Shutdown() { }
		static void Init()
		{
			s_Library = new Library();
		}

		UInt CreateOrGetDescriptorWithUUID_Texture(UUID uuid);
		UInt CreateOrGetDescriptorWithUUID_Layout(UUID uuid);
		UInt CreateOrGetDescriptorWithUUID_Framebuffer(UUID uuid);
		UInt CreateOrGetDescriptorWithUUID_TextureSampler(UUID uuid);
		UInt CreateOrGetDescriptorWithUUID_UniformBufferSampler(UUID uuid);
		UInt CreateOrGetDescriptorWithUUID_ShaderInterface(UUID uuid);
		UInt CreateOrGetDescriptorWithUUID_ProgramSpecification(UUID uuid);

		static Library* s_Library;
	};
}