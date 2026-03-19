#include "FE_pch.h"
#include "Array.h"
#include "Pile.h"
#include "Xar.h"

#include <memory_resource>
#include <vector>
#include <string>

namespace fe
{
	Byte	Pile::s_Buffer[Pile::s_BufferSize];
	U64		Pile::s_RollbackFlags;
	Byte*	Pile::s_Free = s_Buffer;
	U64		Pile::s_Count = 0;
#ifdef FE_INTERNAL_BUILD
	Byte*	Pile::s_MaxFree = s_Buffer;
#endif // FE_INTERNAL_BUILD


	//testing
	int y(DynArr<int>* test) { int x = 0; return x; }
	int z(Xarr<int>* test) { int x = 0; return x; }

	void x()
	{
		static DynArrAlloc<int, MallocAllocator> test;

		volatile auto x = y(&test);

		NullAllocator a;
		ArenaAllocator b;
		FallbackAllocator<ArenaAllocator, NullAllocator> c;
		MallocAllocator d;
		SegragatorAllocator< NullAllocator, NullAllocator, 8> e;
		BitmappedPoolAllocator<8, 8> f;

		auto test2 = MakeXarr<int>(d);

		x = z(&test2);
	}
}