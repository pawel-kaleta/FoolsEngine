#include "FE_pch.h"

#include "Data.h"
#include "Texture.h"
#include "Library.h"
#include "Framebuffer.h"

namespace fe::Description
{
	Library* Library::s_Library;

	void Buffer::Layout::CalculateOffsetsAndStride()
	{
		FE_PROFILER_FUNC();

		FE_CORE_ASSERT(Type == LayoutType::Vertex, "Unsupported LayoutType");

		UInt offset = 0;
		Stride = 0;

		for (UInt i = 0; i < Elements.Count; i++)
		{
			const auto& element = Elements[i];
			UInt size = element.Size();

			Offsets[i] = offset;

			offset += size;
			Stride += size;
		}
	}

	static UInt CreateDefaultVertexLayout()
	{
		auto& lib = Library::Get();
		auto alloc = (TypedAlloc<MonotonicAlloc>*) & lib.m_AllocPermanent;

		UInt id = lib.BufferLayouts.Count;
		auto& layout = *lib.BufferLayouts.PushBack();
		
		layout.Elements.FromArray(alloc->Allocate<Buffer::Element, 5>());

		layout.Elements[0].Type = Data::Type::Float3;
		layout.Elements[0].Name.FromConstCharPtr("a_Position", 11);

		layout.Elements[1].Type = Data::Type::Float3;
		layout.Elements[1].Name.FromConstCharPtr("a_Normal", 9);

		layout.Elements[2].Type = Data::Type::Float3;
		layout.Elements[2].Name.FromConstCharPtr("a_Tangent", 10);

		layout.Elements[3].Type = Data::Type::Float2;
		layout.Elements[3].Name.FromConstCharPtr("a_UV0", 6);

		layout.Elements[4].Type = Data::Type::Float2;
		layout.Elements[4].Name.FromConstCharPtr("a_UV1", 6);

		layout.Type = Buffer::LayoutType::Vertex;
		layout.CalculateOffsetsAndStride();

		return id;
	}

	const Buffer::Layout& Buffer::Vertex::GetLayout()
	{
		const static Buffer::Layout& layout = Library::Get().BufferLayouts[CreateDefaultVertexLayout()];

		return layout;
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

	UInt Library::CreateOrGetDescriptorWithUUID_Layout(UUID uuid)
	{
		auto search_result = UUIDToIDMap.find(uuid);
		if (search_result != UUIDToIDMap.end())
			return search_result->second;
		
		UInt size = BufferLayouts.Count;
		BufferLayouts.PushBack();
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

	UInt Library::CreateOrGetDescriptorWithUUID_TextureSampler(UUID uuid)
	{
		auto search_result = UUIDToIDMap.find(uuid);
		if (search_result != UUIDToIDMap.end())
			return search_result->second;

		UInt size = TextureSamplers.Count;
		TextureSamplers.PushBack();
		return size;
	}

	UInt Library::CreateOrGetDescriptorWithUUID_UniformBufferSampler(UUID uuid)
	{
		auto search_result = UUIDToIDMap.find(uuid);
		if (search_result != UUIDToIDMap.end())
			return search_result->second;

		UInt size = UniformBufferSamplers.Count;
		UniformBufferSamplers.PushBack();
		return size;
	}

	UInt Library::CreateOrGetDescriptorWithUUID_ShaderInterface(UUID uuid)
	{
		auto search_result = UUIDToIDMap.find(uuid);
		if (search_result != UUIDToIDMap.end())
			return search_result->second;
		
		UInt size = ShaderSpecs.Count;
		ShaderSpecs.PushBack();
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