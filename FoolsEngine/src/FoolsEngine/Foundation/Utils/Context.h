#pragma once

#include "FoolsEngine/Foundation/Memory/Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Memory/Allocators/ArenaAlloc.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MallocAlloc.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MonotonicAlloc.h"


namespace fe::Context
{
	namespace Allocators
	{
		//extern PageAllocator					Page;
		//extern VirtualAllocator					Virtual;
		extern TypedAlloc<MonotonicAlloc>		Permanent;
		extern TypedAlloc<MallocAlloc>			GeneralPurpose;
		extern TypedAlloc<ArenaAlloc>			MainLoopArena;
		extern PMAlloc						*	Default;
		extern PMAlloc						*	Auxiliary;
		extern PMAlloc						*	Output;
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