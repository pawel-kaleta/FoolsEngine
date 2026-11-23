#pragma once

#include "Allocator.h"
#include "FoolsEngine/Foundation/Debug/Asserts.h"
#include "FoolsEngine/Foundation/Utils/Core.h"
#include "FoolsEngine/Foundation/Utils/BitOperations.h"

#include <memory_resource>

namespace fe
{
	class MonotonicAlloc : public Allocator
	{
	public:
		STD_PMR_Allocator<Allocator> UpStreamAlloc;
		union { std::pmr::monotonic_buffer_resource MBR; };

		MonotonicAlloc() {};
		~MonotonicAlloc() {};

		void Init(Allocator* upStreamAlloc)
		{
			new (&UpStreamAlloc) STD_PMR_Allocator<Allocator>();
			UpStreamAlloc.fe_allocator = upStreamAlloc;
			new(&MBR) std::pmr::monotonic_buffer_resource(&UpStreamAlloc);
		}
		void Deinit()
		{
			MBR.~monotonic_buffer_resource();
			UpStreamAlloc = nullptr;
		}

		virtual Splice<Byte> AllocateRaw(UInt bytes) final override
		{
			Splice<Byte> mem_reg;
			mem_reg.Elements = (Byte*)MBR.allocate(bytes);
			mem_reg.Count = bytes;
			return mem_reg;
		}

		virtual Splice<Byte> AllocateRaw(UInt bytes, UInt alignment) final override
		{
			Splice<Byte> mem_reg;
			mem_reg.Elements = (Byte*)MBR.allocate(bytes, alignment);
			mem_reg.Count = bytes;
			return mem_reg;
		}
		virtual void DeallocateRaw(Splice<Byte> memReg) final override;

		virtual Splice<Byte> ReallocateRaw(Splice<Byte> allocation, Splice<Byte> moveRegion, UInt newSize, UInt alignment) override final;

		template <UInt Alignment>
		Splice<Byte> ReallocateRaw(Splice<Byte> allocation, Splice<Byte> moveRegion, UInt newSize)
		{
			return ReallocateRaw(allocation, allocation, newSize, Alignment);
		};

		template <UInt Alignment>
		Splice<Byte> ReallocateRaw(Splice<Byte> allocation, UInt newSize)
		{
			return ReallocateRaw(allocation, allocation, newSize, Alignment);
		};

		template <UInt Size, UInt Alignment>
		Array<Byte, Size>* AllocateRaw() { return (Array<Byte, Size>*)MBR.allocate(Size, Alignment); }

		template <UInt Alignment>
		Splice<Byte> AllocateRaw(UInt size) { return AllocateRaw(size, Alignment); }

		template <UInt Size>
		void DeallocateRaw(Byte* ptr) { DeallocateRaw(Splice<Byte>()); }

		void Clear() { MBR.release(); }
	};
}