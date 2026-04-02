#pragma once

#include "Texture.h"

#include "FoolsEngine/Foundation/Memory/String.h"
#include "FoolsEngine/Foundation/Memory/Splice.h"
#include "FoolsEngine/Application/UUID.h"

namespace fe
{
	namespace Description::Framebuffer
	{
		struct Attachment
		{
			String Name;
			Texture::Format Format = Texture::Format::None;
		};

		struct Specification
		{
			UUID UUID = fe::UUID();
			Splice<Attachment> ColorAttachments;
			Texture::Format DepthStencilFormat = Texture::Format::None;
			U32 Width = 0;
			U32 Height = 0;
			U08 Samples = 1;
			bool DynamicSize = false;
		};
	}
}