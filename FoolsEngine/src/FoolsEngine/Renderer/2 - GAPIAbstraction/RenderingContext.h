#pragma once

#include "FoolsEngine\Renderer\1 - Description\GAPIType.h"

namespace fe
{
	using RenderingContextID = uint32_t;
	constexpr RenderingContextID NullRenderingContextID = -1;

	class RenderingContext
	{
	public:
		virtual ~RenderingContext() = default;

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		GAPIType GetGAPIType() { return m_Type; }
		RenderingContextID GetID() { return m_ID; }

		static Scope<RenderingContext> Create(GAPIType GAPI, void* nativeWindow);

		GAPIType m_Type = GAPIType::None;
		RenderingContextID m_ID = -1;
		void* m_Window = nullptr;
		
		static uint32_t ContextsCount;
		
	protected:
		
		RenderingContext() = default;
		RenderingContext(GAPIType GAPI, void* nativeWindow) :
			m_Type(GAPI), m_ID(ContextsCount++), m_Window(nativeWindow)
		{ };
	};
}