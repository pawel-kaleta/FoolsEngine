#pragma once

#include "DataTypes.h"
#include "Splice.h"
#include "FoolsEngine/Foundation/Debug/Asserts.h"
#include "FoolsEngine/Foundation/Utils/BitOperations.h"

namespace fe
{
	template <UInt Count>
	class Bitset
	{
	public:
		static_assert(!(Count % 64), "Biset class only supports powers 64 bits");

		static constexpr UInt WordsCount = Count / 64;

		U64 Data[WordsCount];

		void Init(bool initValue = false)
		{
			if (initValue)
				std::memset(Data.begin(), -1, WordsCount * 8);
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

		bool Any()
		{
			for (UInt word = 0; word < WordsCount; ++word) {
				if (Data[word] != 0) {
					return true;
				}
			}

			return false;
		}

		bool All()
		{
			for (UInt word = 0; word < WordsCount; ++word) {
				if (Data[word] != -1) {
					return false;
				}
			}

			return true;
		}

		UInt FirstTrue()
		{
			for (UInt word = 0; word < WordsCount; ++word) {
				if (Data[word] != 0) {
					unsigned long index = -1;
					MSB64(&index, Data[word]);
					return word * 64 + index;
				}
			}

			return -1;
		}

		UInt FirstFalse()
		{
			for (UInt word = 0; word < WordsCount; ++word) {
				if (Data[word] != -1) {
					unsigned long index = -1;
					MSB64(&index, ~Data[word]);
					return word * 64 + index;
				}
			}

			return -1;
		}
	};
}