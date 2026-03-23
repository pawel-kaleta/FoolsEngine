#pragma once

#include "FoolsEngine/Foundation/Memory/Allocator.h"

namespace fe::Context
{
	namespace Allocators
	{
		extern Allocator* GeneralPurpose;
		extern Allocator* Default;
		extern Allocator* Permanent;
		extern Allocator* Temporary;
		extern Allocator* SystemOutput;
		void GetExlusiveArena() { };
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