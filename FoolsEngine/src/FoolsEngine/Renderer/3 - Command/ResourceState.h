#pragma once

#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"
#include "FoolsEngine/Renderer/2 - Resource/RProgram.h"
#include "FoolsEngine/Renderer/2 - Resource/RFramebuffer.h"
#include "FoolsEngine/Renderer/2 - Resource/RBuffer.h"

#include "FoolsEngine/Foundation/Memory/String.h"

namespace fe::Command
{
	struct ResourceState
	{
		virtual void Clear() = 0;

		virtual void AllocateBuffer(Resource::RBuffer* buffer, UInt size) = 0;
	};

	struct ResourceState_OpenGL final : public ResourceState
	{
		virtual void Clear() final override;

		virtual void AllocateBuffer(Resource::RBuffer* buffer, UInt size) final override;
	};
}