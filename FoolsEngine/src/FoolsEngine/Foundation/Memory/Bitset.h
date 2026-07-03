#pragma once

#include "DataTypes.h"
#include "Splice.h"
#include "FoolsEngine/Foundation/Debug/Asserts.h"

namespace fe
{
	template <UInt Count>
	class Bitset
	{
	public:
		static_assert(Count > 64, "Biset class only supports more then 64 bits");

		static constexpr UInt WordsCount = (Count - 1) / 64 + 1;

		Array<U64, WordsCount> Data;

		void Init(bool initValue = false)
		{
			if (initValue)
				std::memset(Data.begin(), -1, WordsCount * 8);;
			else
				std::memset(Data.begin(), 0, WordsCount * 8);
		}

		bool At(UInt i) const
		{
			FE_CORE_ASSERT(i < Count, "I out of bounds in Bitset");
			return (Data[i / 64] & (U64{ 1 } << i % 64));
		}

		void Set(UInt i, bool val = true)
		{
			FE_CORE_ASSERT(i < Count, "I out of bounds in Bitset");

			auto& word = Data[i / 64];
			const auto bit = U64{ 1 } << i % 64;
			if (val)
				word |=  bit;
			else
				word &= ~bit;
		}
	};
}