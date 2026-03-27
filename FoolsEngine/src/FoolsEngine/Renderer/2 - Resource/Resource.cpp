#include "FE_pch.h"

#include "FoolsEngine/Renderer/1 - Description/Library.h"
#include "FoolsEngine/Renderer/2 - Resource/Program.h"
#include "FoolsEngine/Renderer/2 - Resource/Framebuffer.h"

namespace fe::Resource
{
	UInt FramebufferBase::GetColorAttachmentIndex(String name) const
	{
		const auto& spec = Description::Library::Get().FramebufferSpecs[SpecificationID];

		for (UInt i = 0; i < spec.ColorAttachments.Count; ++i)
		{
			auto& attachment = spec.ColorAttachments[i];
			if (attachment.Name.Length != name.Length)
				continue;

			if (!std::memcmp(attachment.Name.Data, name.Data, name.Length))
				continue;

			return i;
		}

		FE_CORE_ASSERT(false, "Attachment {0} not found in framebuffer", name);
		return 0;
	}
}