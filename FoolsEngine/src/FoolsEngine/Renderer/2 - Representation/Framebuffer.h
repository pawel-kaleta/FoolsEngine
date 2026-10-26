#pragma once

#include "FoolsEngine/Foundation/Utils/DeclareEnum.h"
#include "FoolsEngine/Foundation/Containers/Arena.h"

#include "foolsEngine/Renderer/1 - GAPI/Resource.h"
#include "foolsEngine/Renderer/1 - GAPI/Pipeline.h"


#include <glm/glm.hpp>

namespace fe::Render::Representation
{
	struct Framebuffer
	{
		struct Attachment
		{
			GAPI::Descriptors::TextureFormat mFormat = GAPI::Descriptors::TextureFormat::None;
			GAPI::GID mGID;

			void Init()
			{
				mFormat = GAPI::Descriptors::TextureFormat::None;
				mGID = GAPI::GID();
			}
		};

		ArrayArena<Attachment, 6>	mColorAttachments;
		Attachment					mDepthStencilAttachment;
		glm::vec<2, U32>			mDimentions;

		Framebuffer& Init()
		{
			mColorAttachments.Init();
			mDepthStencilAttachment.Init();
			mDimentions = { UInt(1), UInt(1) };
			return *this;
		}
	};
}