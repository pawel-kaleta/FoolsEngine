#include "FE_pch.h"

#include "FoolsEngine/Renderer/1 - Description/Library.h"
#include "FoolsEngine/Renderer/2 - Resource/RProgram.h"
#include "FoolsEngine/Renderer/2 - Resource/RFramebuffer.h"

namespace fe::Resource
{
	UInt RFramebuffer::GetColorAttachmentIndex(String name) const
	{
		const auto& spec = Description::Library::Get().FramebufferSpecs[SpecificationID];

		for (UInt i = 0; i < spec.ColorAttachments.Count; ++i)
		{
			auto& attachment = spec.ColorAttachments[i];
			if (CompareStringsEqual(attachment.Name, name))
				return i;
		}

		FE_CORE_ASSERT(false, "Attachment {0} not found in framebuffer", name.CData());
		return 0;
	}
}