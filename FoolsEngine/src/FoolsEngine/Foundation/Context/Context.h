#pragma once

#include "ContextStruct.h"
#include "GlobalContextStruct.h"

namespace fe
{
	extern ContextStruct* Context;
	extern GlobalContextStruct* GlobalContext;

	namespace ContextScope
	{
		namespace Alloc
		{
			class Default
			{
			public:
				Default(PMAlloc* alloc) : mBackup(Context->Alloc.Default) { Context->Alloc.Default = alloc; }
				~Default() { Context->Alloc.Default = mBackup; }
			private:
				PMAlloc* mBackup;
			};

			class Auxiliary
			{
			public:
				Auxiliary(PMAlloc* alloc) : mBackup(Context->Alloc.Auxiliary) { Context->Alloc.Auxiliary = alloc; }
				~Auxiliary() { Context->Alloc.Auxiliary = mBackup; }
			private:
				PMAlloc* mBackup;
			};

			class Output
			{
			public:
				Output(PMAlloc* alloc) : mBackup(Context->Alloc.Output) { Context->Alloc.Output = alloc; }
				~Output() { Context->Alloc.Output = mBackup; }
			private:
				PMAlloc* mBackup;
			};
		}

		namespace Log
		{
			class FE
			{
			public:
				FE(String prefix, LoggingLevel lvl = Context->Log.FE.mLoggingLevel) :
					mPrefixCount(prefix.Count),
					mLvlBackup(Context->Log.FE.mLoggingLevel)
				{
					auto& suffixes = Context->Log.FE.mSuffixes;
					auto& suffixes_count = Context->Log.FE.mSuffixesCount;

					suffixes[suffixes_count] = '[';
					std::memcpy(&suffixes[suffixes_count + 1], prefix.Data, prefix.Count);
					suffixes[suffixes_count + 1 + prefix.Count] = ']';
					suffixes_count += prefix.Count + 2;
					Context->Log.FE.mLoggingLevel = lvl;
				}
				~FE()
				{
					Context->Log.FE.mSuffixesCount -= mPrefixCount + 2;
					Context->Log.FE.mLoggingLevel = mLvlBackup;
				}
			private:
				UInt mPrefixCount;
				LoggingLevel mLvlBackup;
			};

			class AP
			{
			public:
				AP(String prefix, LoggingLevel lvl = Context->Log.AP.mLoggingLevel) :
					mPrefixCount(prefix.Count),
					mLvlBackup(Context->Log.AP.mLoggingLevel)
				{
					auto& suffixes = Context->Log.AP.mSuffixes;
					auto& suffixes_count = Context->Log.AP.mSuffixesCount;

					suffixes[suffixes_count] = '[';
					std::memcpy(&suffixes[suffixes_count + 1], prefix.Data, prefix.Count);
					suffixes[suffixes_count + 1 + prefix.Count] = ']';
					suffixes_count += prefix.Count + 2;
					Context->Log.AP.mLoggingLevel = lvl;
				}
				~AP()
				{
					Context->Log.AP.mSuffixesCount -= mPrefixCount + 2;
					Context->Log.AP.mLoggingLevel = mLvlBackup;
				}
			private:
				UInt mPrefixCount;
				LoggingLevel mLvlBackup;
			};

			class ED
			{
			public:
				ED(String prefix, LoggingLevel lvl = Context->Log.ED.mLoggingLevel) :
					mPrefixCount(prefix.Count),
					mLvlBackup(Context->Log.ED.mLoggingLevel)
				{
					auto& suffixes = Context->Log.ED.mSuffixes;
					auto& suffixes_count = Context->Log.ED.mSuffixesCount;

					suffixes[suffixes_count] = '[';
					std::memcpy(&suffixes[suffixes_count + 1], prefix.Data, prefix.Count);
					suffixes[suffixes_count + 1 + prefix.Count] = ']';
					suffixes_count += prefix.Count + 2;
					Context->Log.ED.mLoggingLevel = lvl;
				}
				~ED()
				{
					Context->Log.ED.mSuffixesCount -= mPrefixCount + 2;
					Context->Log.ED.mLoggingLevel = mLvlBackup;
				}
			private:
				UInt mPrefixCount;
				LoggingLevel mLvlBackup;
			};
		}

		// namespace Debug { // assertion failure handler }
		// namespace Rand { }
		// namespace GPU { }
	}
}

#include "FoolsEngine/Foundation/Debug/Asserts.h"