#include "FE_pch.h"
#include "Array.h"

#include <memory_resource>
#include <vector>
#include <string>

namespace fe
{
	int y(DynArr<int>* test) { int x = 0; return x; }

	void x()
	{
		static DynArrStat<int, MallocAllocator> test;

		volatile auto x = y(&test);

		NullAllocator a;
		ArenaAllocator b;
		FallbackAllocator<ArenaAllocator, NullAllocator> c;
		MallocAllocator d;
		SegragatorAllocator< NullAllocator, NullAllocator, 8> e;
		BitmappedPoolAllocator<8, 8> f;
	}


}