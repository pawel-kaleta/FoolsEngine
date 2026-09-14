#include "FE_pch.h"

#include "Data.h"
#include "Texture.h"
#include "Library.h"
#include "Framebuffer.h"

namespace fe::Description
{
	Library* Library::s_Library;

	Library::Library() :
		m_AllocPermanent(),
		m_AllocGPA(),
		m_AllocGPA_STD_PMR(&m_AllocGPA),

		UUIDToIDMap(&m_AllocGPA_STD_PMR)
	{
		TextureArchetypes.InitXarrAlloc(&m_AllocPermanent, &m_AllocGPA);
		FramebufferSpecs.InitXarrAlloc(&m_AllocPermanent, &m_AllocGPA);
		PipelineStates.InitXarrAlloc(&m_AllocPermanent, &m_AllocGPA);
		ProgramSpecs.InitXarrAlloc(&m_AllocPermanent, &m_AllocGPA);
	}

	UInt Library::CreateOrGetDescriptorWithUUID_Texture(UUID uuid)
	{
		auto search_result = UUIDToIDMap.find(uuid);
		if (search_result != UUIDToIDMap.end())
			return search_result->second;
		
		UInt size = TextureArchetypes.Count;
		TextureArchetypes.PushBack();
		return size;
	}

	UInt Library::CreateOrGetDescriptorWithUUID_Framebuffer(UUID uuid)
	{
		auto search_result = UUIDToIDMap.find(uuid);
		if (search_result != UUIDToIDMap.end())
			return search_result->second;
		
		UInt size = FramebufferSpecs.Count;
		FramebufferSpecs.PushBack();
		return size;
	}

	UInt Library::CreateOrGetDescriptorWithUUID_Pipeline(UUID uuid)
	{
		auto search_result = UUIDToIDMap.find(uuid);
		if (search_result != UUIDToIDMap.end())
			return search_result->second;

		UInt size = PipelineStates.Count;
		PipelineStates.PushBack();
		return size;
	}

	UInt Library::CreateOrGetDescriptorWithUUID_ProgramSpecification(UUID uuid)
	{
		auto search_result = UUIDToIDMap.find(uuid);
		if (search_result != UUIDToIDMap.end())
			return search_result->second;

		UInt size = ProgramSpecs.Count;
		ProgramSpecs.PushBack();
		return size;
	}
}