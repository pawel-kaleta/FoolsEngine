#pragma once

#include "FoolsEngine/Foundation/Debug/Logger.h"

#include "FoolsEngine/Foundation/Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Allocators/StaticArenaAlloc.h"
#include "FoolsEngine/Foundation/Allocators/MallocAlloc.h"
#include "FoolsEngine/Foundation/Allocators/MonotonicAlloc.h"

#include "FoolsEngine/Renderer/1 - GAPI/Context.h"

#include "FoolsEngine/Platform/PlatformBase.h"

namespace fe::Context
{
	namespace Alloc
	{
		extern TypedAlloc<MonotonicAlloc>		Permanent;
		extern TypedAlloc<MallocAlloc>			GeneralPurpose;
		extern TypedAlloc<StaticArenaAlloc
				<FE_PARAMS_MAIN_ARENA_SIZE>>	MainLoopArena;

		extern PMAlloc						*	Default;
		extern PMAlloc						*	Auxiliary;
		extern PMAlloc						*	Output;

		class ScopeDefault
		{
		public:
			ScopeDefault(PMAlloc* alloc) : mBackup(Default) { Default = alloc; }
			~ScopeDefault() { Default = mBackup; }
		private:
			PMAlloc* mBackup;
		};

		class ScopeAuxiliary
		{
		public:
			ScopeAuxiliary(PMAlloc* alloc) : mBackup(Default) { Default = alloc; }
			~ScopeAuxiliary() { Default = mBackup; }
		private:
			PMAlloc* mBackup;
		};

		class ScopeOutput
		{
		public:
			ScopeOutput(PMAlloc* alloc) : mBackup(Default) { Default = alloc; }
			~ScopeOutput() { Default = mBackup; }
		private:
			PMAlloc* mBackup;
		};
	}

	namespace Log
	{
		extern Logger Core;
		extern Logger App;

		class ScopeCore
		{
		public:
			ScopeCore(String prefix, LoggingLevel lvl = Core.mLoggingLevel) :
				mPrefixCount(prefix.Count),
				mLvlBackup(Core.mLoggingLevel)
			{
				Core.mSuffixes[Core.mSuffixesCount] = '[';
				std::memcpy(&Core.mSuffixes[Core.mSuffixesCount + 1], prefix.Data, prefix.Count);
				Core.mSuffixes[Core.mSuffixesCount + 1 + prefix.Count] = ']';
				Core.mSuffixesCount += prefix.Count + 2;
				Core.mLoggingLevel = lvl;
			}
			~ScopeCore()
			{
				Core.mSuffixesCount -= mPrefixCount + 2;
				Core.mLoggingLevel = mLvlBackup;
			}
		private:
			UInt mPrefixCount;
			LoggingLevel mLvlBackup;
		};
	}

	namespace Debug
	{
		// assertion failure handler
	}

	namespace Rand
	{

	}

	namespace GPU
	{
		using namespace Render;
		extern GAPI::GID ScratchBuffer;
		extern U32 ScratchBufferSize;
	}

}