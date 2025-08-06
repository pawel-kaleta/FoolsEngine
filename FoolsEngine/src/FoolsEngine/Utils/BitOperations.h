#pragma once

#include <stdint.h>
#include <intrin.h>

namespace fe
{
#define BIT_FLAG(x) (1 << x)
#define WIDE_BIT_FLAG(x) ((uint64_t)1 << x)

	unsigned char MSB64(unsigned long* outIndex, unsigned long long mask) { return _BitScanReverse64(outIndex, mask); }
	unsigned char MSB32(unsigned long* outIndex, unsigned long mask) { return _BitScanReverse(outIndex, mask); }
}