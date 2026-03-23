#pragma once

#include "FoolsEngine/Foundation/Memory/Allocator.h"

namespace fe
{
	namespace Context
	{
		namespace Allocators
		{
			namespace Global
			{
				extern Allocator* GeneralPurpose;
				extern Allocator* Permanent;
				extern Allocator* Temporary;
				void GetExlusiveArena() {};
			}
			namespace System
			{
				extern Allocator* GeneralPurpose;
				extern Allocator* Permanent;
				extern Allocator* Temporary;
				extern Allocator* OutputAllocator;
			}
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
}