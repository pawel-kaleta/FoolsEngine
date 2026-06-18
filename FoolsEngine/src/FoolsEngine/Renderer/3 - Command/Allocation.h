#pragma once

#include "FoolsEngine/Renderer/2 - Resource/RStaticBuffer.h"

namespace fe::Command::Allocation
{
	class Allocator
	{
	public:
		virtual Resource::RMemReg AllocateRaw(UInt bytes) = 0;
		virtual Resource::RMemReg AllocateRaw(UInt bytes, UInt alignment) = 0;
		virtual void DeallocateRaw(Resource::RMemReg) = 0;

		template <UInt Size, UInt Alignment>
		Array<Byte, Size>* AllocateRaw() { return (Array<Byte, Size>*)AllocateRaw(Size, Alignment).Elements; }

		template <UInt Alignment>
		Splice<Byte> AllocateRaw(UInt bytes) { return AllocateRaw(bytes, Alignment); }

		template <UInt Size>
		void DeallocateRaw(Byte* ptr)
		{
			Splice<Byte> memReg;
			memReg.Elements = ptr;
			memReg.Count = Size;
			DeallocateRaw(memReg);
		}
	};

	struct Render
	{
		virtual void DrawIndexed(const Resource::RMeshBindings& meshBindings) = 0;
	};

	struct Render_OpenGL final : public Render
	{

		virtual void DrawIndexed(const Resource::RMeshBindings& meshBindings) final override;
	};
}