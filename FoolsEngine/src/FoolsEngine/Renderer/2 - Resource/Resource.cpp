#include "FE_pch.h"

#include "FoolsEngine/Renderer/1 - Description/Library.h"
#include "FoolsEngine/Renderer/2 - Resource/Program.h"
#include "FoolsEngine/Renderer/2 - Resource/Framebuffer.h"

namespace fe::Resource
{
	uint32_t FramebufferBase::GetColorAttachmentIndex(const std::string& name) const
	{
		const auto& spec = Description::Library::Get().FramebufferSpecs[SpecificationID];

		for (int i = 0; i < spec.ColorAttachments.size(); ++i)
		{
			if (spec.ColorAttachments[i].Name.compare(name))
				return i;
		}

		FE_CORE_ASSERT(false, "Attachment {0} not found in framebuffer", name);
		return 0;
	}
}