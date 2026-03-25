#pragma once

#include "FoolsEngine/Foundation/Memory/Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MallocAllocator.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MonotonicAllocator.h"



namespace fe::Context
{
	namespace Allocators
	{
		extern Allocator* Default;
		extern Allocator* Auxiliary;
		extern Allocator* Temporary;
		extern Allocator* Output;
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