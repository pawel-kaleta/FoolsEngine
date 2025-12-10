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

		std::pmr::vector<Texture::Specification>				TextureSpecs;
		std::pmr::vector<Buffer::Layout>						BufferLayouts;
		std::pmr::vector<Framebuffer::Specification>			FramebufferSpecs;
		std::pmr::vector<ShaderInterface::TextureSampler>		TextureSamplers;
		std::pmr::vector<ShaderInterface::UniformBufferSampler>	UniformBufferSamplers;
		std::pmr::vector<ShaderInterface::Specification>		ShaderSpecs;
		std::pmr::vector<ShaderInterface::ProgramSpecification>	ProgramSpecs;
		
		std::pmr::map<UUID, uint32_t> UUIDToIDMap;
		static Library& Get() { return *s_Library; }

		template <typename tnDescriptor>
		tnDescriptor& CreateOrGetDescriptorWithUUID(UUID uuid)
		{
			if constexpr (std::same_as<tnDescriptor, Texture::Specification					>) return CreateOrGetDescriptorWithUUID_Texture(uuid);
			if constexpr (std::same_as<tnDescriptor, Buffer::Layout							>) return CreateOrGetDescriptorWithUUID_Layout(uuid);
			if constexpr (std::same_as<tnDescriptor, Framebuffer::Specification				>) return CreateOrGetDescriptorWithUUID_Framebuffer(uuid);
			if constexpr (std::same_as<tnDescriptor, ShaderInterface::TextureSampler		>) return CreateOrGetDescriptorWithUUID_TextureSampler(uuid);
			if constexpr (std::same_as<tnDescriptor, ShaderInterface::UniformBufferSampler	>) return CreateOrGetDescriptorWithUUID_UniformBufferSampler(uuid);
			if constexpr (std::same_as<tnDescriptor, ShaderInterface::Specification			>) return CreateOrGetDescriptorWithUUID_ShaderInterface(uuid);
			if constexpr (std::same_as<tnDescriptor, ShaderInterface::ProgramSpecification	>) return CreateOrGetDescriptorWithUUID_ProgramSpecification(uuid);
		}
		
	private:
		Library() :
			m_Allocator(),
			TextureSpecs(&m_Allocator),
			BufferLayouts(&m_Allocator),
			FramebufferSpecs(&m_Allocator),
			TextureSamplers(&m_Allocator),
			UniformBufferSamplers(&m_Allocator),
			ShaderSpecs(&m_Allocator),
			ProgramSpecs(&m_Allocator),
			UUIDToIDMap(&m_Allocator)
		{ }

		friend class Application;
		static void Shutdown() { }
		static void Init()
		{
			s_Library = new Library();
		}

		Texture::Specification&					CreateOrGetDescriptorWithUUID_Texture(UUID uuid);
		Buffer::Layout&							CreateOrGetDescriptorWithUUID_Layout(UUID uuid);
		Framebuffer::Specification&				CreateOrGetDescriptorWithUUID_Framebuffer(UUID uuid);
		ShaderInterface::TextureSampler&		CreateOrGetDescriptorWithUUID_TextureSampler(UUID uuid);
		ShaderInterface::UniformBufferSampler&	CreateOrGetDescriptorWithUUID_UniformBufferSampler(UUID uuid);
		ShaderInterface::Specification&			CreateOrGetDescriptorWithUUID_ShaderInterface(UUID uuid);
		ShaderInterface::ProgramSpecification&	CreateOrGetDescriptorWithUUID_ProgramSpecification(UUID uuid);

		static Library* s_Library;
	};
}