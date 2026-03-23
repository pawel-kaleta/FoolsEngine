#pragma once

#include "Buffer.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "ShaderInterface.h"

#include "FoolsEngine/Foundation/Common.h"
#include "FoolsEngine/Foundation/Memory/Xar.h"

#include <memory_resource>

namespace fe::Description
{
	struct Library
	{
		MonotonicAllocator	m_AllocPermanent;
		MallocAllocator		m_AllocGPA;
		STD_PMR_Allocator<MallocAllocator>	m_AllocGPA_STD_PMR;

		XarrAlloc<Texture::Archetype,						MonotonicAllocator>	TextureArchetypes;
		XarrAlloc<Buffer::Layout,							MonotonicAllocator>	BufferLayouts;
		XarrAlloc<Framebuffer::Specification,				MonotonicAllocator>	FramebufferSpecs;
		XarrAlloc<ShaderInterface::TextureSampler,			MonotonicAllocator>	TextureSamplers;
		XarrAlloc<ShaderInterface::UniformBufferSampler,	MonotonicAllocator>	UniformBufferSamplers;
		XarrAlloc<ShaderInterface::Specification,			MonotonicAllocator>	ShaderSpecs;
		XarrAlloc<ShaderInterface::ProgramSpecification,	MonotonicAllocator>	ProgramSpecs;
		
		std::pmr::map<UUID, U32> UUIDToIDMap;
		static Library& Get() { return *s_Library; }

		template <typename tnDescriptor>
		UInt CreateOrGetDescriptorWithUUID(UUID uuid)
		{
			Context::ValueBackup<Allocator*>(&Context::Allocators::System::GeneralPurpose, & m_AllocGPA);
			if constexpr (std::same_as<tnDescriptor, Texture::Archetype						>) return CreateOrGetDescriptorWithUUID_Texture(uuid);
			if constexpr (std::same_as<tnDescriptor, Buffer::Layout							>) return CreateOrGetDescriptorWithUUID_Layout(uuid);
			if constexpr (std::same_as<tnDescriptor, Framebuffer::Specification				>) return CreateOrGetDescriptorWithUUID_Framebuffer(uuid);
			if constexpr (std::same_as<tnDescriptor, ShaderInterface::TextureSampler		>) return CreateOrGetDescriptorWithUUID_TextureSampler(uuid);
			if constexpr (std::same_as<tnDescriptor, ShaderInterface::UniformBufferSampler	>) return CreateOrGetDescriptorWithUUID_UniformBufferSampler(uuid);
			if constexpr (std::same_as<tnDescriptor, ShaderInterface::Specification			>) return CreateOrGetDescriptorWithUUID_ShaderInterface(uuid);
			if constexpr (std::same_as<tnDescriptor, ShaderInterface::ProgramSpecification	>) return CreateOrGetDescriptorWithUUID_ProgramSpecification(uuid);
		}
		
	private:
		Library() :
			m_AllocPermanent(),
			m_AllocGPA(),
			m_AllocGPA_STD_PMR(&m_AllocGPA),
			TextureArchetypes(&m_AllocPermanent),
			BufferLayouts(&m_AllocPermanent),
			FramebufferSpecs(&m_AllocPermanent),
			TextureSamplers(&m_AllocPermanent),
			UniformBufferSamplers(&m_AllocPermanent),
			ShaderSpecs(&m_AllocPermanent),
			ProgramSpecs(&m_AllocPermanent),
			UUIDToIDMap(&m_AllocGPA_STD_PMR)
		{ }

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