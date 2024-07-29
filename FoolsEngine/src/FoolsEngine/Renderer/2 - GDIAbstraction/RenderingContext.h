#pragma once

#include "FoolsEngine\Assets\Asset.h"

#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"

namespace fe
{
	using RenderingContextID = uint32_t;
	constexpr uint32_t NullRenderingContextID = -1;

	class RenderingContext
	{
	public:
		virtual ~RenderingContext() = default;

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		GDIType GetGDIType() { return m_Type; }
		RenderingContextID GetID() { return m_ID; }

		static Scope<RenderingContext> Create(GDIType GDI, void* nativeWindow);
	protected:
		GDIType m_Type = GDIType::none;
		RenderingContextID m_ID = -1;
		void* m_Window = nullptr;
		
		static uint32_t ContextsCount;
		
		
		RenderingContext() = default;
		RenderingContext(GDIType GDI, void* nativeWindow) :
			m_Type(GDI), m_ID(ContextsCount++), m_Window(nativeWindow)
		{ };
	};
}