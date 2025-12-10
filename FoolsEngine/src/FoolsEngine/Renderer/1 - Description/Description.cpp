#include "FE_pch.h"

#include "Data.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "Library.h"

namespace fe::Description
{
	Library* Library::s_Library;

	Buffer::Element::Element()
		: Name(& Library::Get().m_Allocator), Type(Data::Type::None), Count(0), Normalized(false) { }

	Buffer::Element::Element(Data::Type type, const std::string& name, uint32_t count, bool normalized)
		: Name(name, & Library::Get().m_Allocator), Type(type), Count(count), Normalized(normalized) { }

	Buffer::Layout::Layout() :
		Type(LayoutType::None),
		Stride(0),
		Elements(& Library::Get().m_Allocator),
		Offsets(& Library::Get().m_Allocator),
		UUID()
	{ }

	void Buffer::Layout::CalculateOffsetsAndStride()
	{
		FE_PROFILER_FUNC();

		FE_CORE_ASSERT(Type == LayoutType::Vertex, "Unsupported LayoutType");

		uint32_t offset = 0;
		Stride = 0;

		for (auto& element : Elements)
		{
			uint32_t size = (uint32_t)element.Size();

			Offsets.push_back(offset);

			offset += size;
			Stride += size;
		}
	}

	static uint32_t CreateDefaultVertexLayout()
	{
		uint32_t id = (uint32_t)Library::Get().BufferLayouts.size();

		auto& layout = Library::Get().BufferLayouts.emplace_back();

		layout.Type = Buffer::LayoutType::Vertex;
		layout.Elements.emplace_back(Data::Type::Float3, "a_Position");
		layout.Elements.emplace_back(Data::Type::Float3, "a_Normal");
		layout.Elements.emplace_back(Data::Type::Float3, "a_Tangent");
		layout.Elements.emplace_back(Data::Type::Float2, "a_UV0");
		layout.Elements.emplace_back(Data::Type::Float2, "a_UV1");

		layout.CalculateOffsetsAndStride();

		return id;
	}

	const Buffer::Layout& Buffer::Vertex::GetLayout()
	{
		static uint32_t layoutID = CreateDefaultVertexLayout();

		return Library::Get().BufferLayouts[layoutID];
	}

	ShaderInterface::TextureSampler::TextureSampler()
		: Spec(), Name(& Library::Get().m_Allocator), UUID() { }

	ShaderInterface::Specification::Specification() :
		Type(ShaderType::None),
		InputLayoutID(-1),
		OutputLayoutID(-1),
		MainUniformsLayoutID(-1),
		TextureSamplerIDs(& Library::Get().m_Allocator),
		UniformBufferSamplerIDs(& Library::Get().m_Allocator),
		DynamicBufferSamplerIDs(& Library::Get().m_Allocator),
		UUID()
	{ }

	ShaderInterface::ProgramSpecification::ProgramSpecification() :
		VertexInputLayoutID(-1),
		VertexOutputLayoutID(-1),
		FragmentOutputLayoutID(-1),
		MainUniformsLayoutID(-1),
		TextureSamplers(& Library::Get().m_Allocator),
		UniformBufferSamplerIDs(& Library::Get().m_Allocator),
		DynamicBufferSamplerIDs(& Library::Get().m_Allocator),
		VertexOutputCapture(false),
		UUID()
	{ }

	Framebuffer::Attachment::Attachment()
		: Name(& Library::Get().m_Allocator), Format(Texture::Format::None) { }

	Framebuffer::Attachment::Attachment(const std::pmr::string& name, Texture::Format format)
		: Name(name, &Library::Get().m_Allocator), Format(format) { }

	Framebuffer::Specification::Specification() :
		Type(Texture::Type::Texture2D),
		Width(), Height(),
		Samples(1),
		SwapChainTarget(false),
		DepthStencilFormat(Texture::Format::None),
		UUID(),
		ColorAttachments(& Library::Get().m_Allocator)
	{ }

	Texture::Specification& Library::CreateOrGetDescriptorWithUUID_Texture(UUID uuid)
	{
		auto search_result = UUIDToIDMap.find(uuid);
		if (search_result != UUIDToIDMap.end())
			return TextureSpecs.emplace_back();
		
		return TextureSpecs[search_result->second];
	}

	Buffer::Layout& Library::CreateOrGetDescriptorWithUUID_Layout(UUID uuid)
	{
		auto search_result = UUIDToIDMap.find(uuid);
		if (search_result != UUIDToIDMap.end())
			return BufferLayouts.emplace_back();

		return BufferLayouts[search_result->second];
	}

	Framebuffer::Specification& Library::CreateOrGetDescriptorWithUUID_Framebuffer(UUID uuid)
	{
		auto search_result = UUIDToIDMap.find(uuid);
		if (search_result != UUIDToIDMap.end())
			return FramebufferSpecs.emplace_back();

		return FramebufferSpecs[search_result->second];
	}

	ShaderInterface::TextureSampler& Library::CreateOrGetDescriptorWithUUID_TextureSampler(UUID uuid)
	{
		auto search_result = UUIDToIDMap.find(uuid);
		if (search_result != UUIDToIDMap.end())
			return TextureSamplers.emplace_back();

		return TextureSamplers[search_result->second];
	}

	ShaderInterface::UniformBufferSampler& Library::CreateOrGetDescriptorWithUUID_UniformBufferSampler(UUID uuid)
	{
		auto search_result = UUIDToIDMap.find(uuid);
		if (search_result != UUIDToIDMap.end())
			return UniformBufferSamplers.emplace_back();

		return UniformBufferSamplers[search_result->second];
	}

	ShaderInterface::Specification& Library::CreateOrGetDescriptorWithUUID_ShaderInterface(UUID uuid)
	{
		auto search_result = UUIDToIDMap.find(uuid);
		if (search_result != UUIDToIDMap.end())
			return ShaderSpecs.emplace_back();

		return ShaderSpecs[search_result->second];
	}

	ShaderInterface::ProgramSpecification& Library::CreateOrGetDescriptorWithUUID_ProgramSpecification(UUID uuid)
	{
		auto search_result = UUIDToIDMap.find(uuid);
		if (search_result != UUIDToIDMap.end())
			return ProgramSpecs.emplace_back();

		return ProgramSpecs[search_result->second];
	}
}