#pragma once

#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"
#include "FoolsEngine/Renderer/2 - Resource/RProgram.h"
#include "FoolsEngine/Renderer/2 - Resource/RFramebuffer.h"

#include "FoolsEngine/Foundation/Memory/String.h"

namespace fe::Command
{
	namespace ResourceState
	{
		template <GAPIType::ValueType GAPI> void Clear();

		template <> void Clear<GAPIType::OpenGL>();
		inline		void Clear_OpenGL() { Clear<GAPIType::OpenGL>(); }
	}
}