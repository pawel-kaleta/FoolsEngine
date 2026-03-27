#pragma once

#include "FoolsEngine/Foundation/Memory/Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MallocAllocator.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MonotonicAllocator.h"



namespace fe::Context
{
	namespace Allocators
	{
		extern TypedAlloc<Allocator>* Default;
		extern TypedAlloc<Allocator>* Auxiliary;
		extern TypedAlloc<Allocator>* Temporary;
		extern TypedAlloc<Allocator>* Output;
	}

	namespace Logging
	{

	}

	namespace Debug
	{
		// assertion failure handler
	}

	namespace Rand
	{

	}

	template <typename T>
	class ValueBackup
	{
	public:
		ValueBackup(T* original, T replacer) : Backup(*original), Location(original) { *original = replacer; }
		~ValueBackup() { *Location = Backup; }
	private:
		T Backup;
		T* Location;
	};
}