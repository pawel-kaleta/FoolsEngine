#pragma once

#include "Splice.h"
#include "Pile.h"
#include "FoolsEngine/Foundation/Utils/Context.h"
#include "FoolsEngine/Foundation/Utils/TypeSetup.h"
#include "FoolsEngine/Foundation/Debug/Log.h"

#include <cstring>

namespace fe
{
	template <typename T, UInt N>
	struct ArrayArena
	{
		UInt Count;
		Array<T, N> Buffer;

		FE_TYPE_SETUP(ArrayArena<T, N>)

		const	T* begin()	const	{ return Buffer.Elements; }
				T* begin()			{ return Buffer.Elements; }
		const	T* end()	const	{ return Buffer.Elements + Count; }
				T* end()			{ return Buffer.Elements + Count; }

		bool IsFull() const { return Count >= Buffer.Count; }

		void Init()
		{
			Count = 0;
		}

		T& operator[](UInt i)
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Buffer[i];
		}

		T& At(UInt i)
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Buffer[i];
		}

		const T& operator[](UInt i) const
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Buffer[i];
		}

		const T& At(UInt i) const
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Buffer[i];
		}

		void Insert(const T& data, UInt index)
		{
			FE_CORE_ASSERT(Count > index, "Index past occupied part of arena");
			FE_CORE_ASSERT(Count < Buffer.Count, "Arena overflow!");

			Pile p;

			UInt move_count = Count - index;
			UInt move_size = move_count * sizeof(T);

			Splice<T> tmp = p.Allocate<T>(move_count);

			std::memcpy(tmp.Elements, & Buffer[index], move_size);
			std::memcpy(& Buffer[index + 1], tmp.Elements, move_size);
			Buffer[index] = data;

			++Count;
		}

		T* Emplace(UInt index)
		{
			FE_CORE_ASSERT(Count > index, "Index past occupied part of arena");
			FE_CORE_ASSERT(Count < Buffer.Count, "Arena overflow!");

			Pile p;

			UInt move_count = Count - index;
			UInt move_size = move_count * sizeof(T);

			Splice<T> tmp = p.Allocate<T>(move_count);

			std::memcpy(tmp.Elements, & Buffer[index], move_size);
			std::memcpy(& Buffer[index + 1], tmp.Elements, move_size);

			++Count;
			return &Buffer[index];
		}

		void Append(const T& data)
		{
			FE_CORE_ASSERT(Count < Buffer.Count, "Arena overflow!");
			Buffer[Count] = data;
			Count++;
		}

		void Append(Splice<T> splice)
		{
			FE_CORE_ASSERT(Count + splice.Count < Buffer.Count, "Arena overflow!");
			FE_CORE_ASSERT(splice.Elements, "Appending invalid splice to ArrayArena");
			std::memcpy(&Buffer[Count], splice.begin(), splice.Count);
			Count += splice.Count;
		}

		T* EmplaceBack()
		{
			FE_CORE_ASSERT(Count < Buffer.Count, "Arena overflow!");
			T* result = Buffer.Elements + Count;
			Count++;
			return result;
		}

		T Pop(UInt index)
		{
			FE_CORE_ASSERT(index < Count, "Index past occupied part of arena");

			T result = Buffer[index];

			Pile p;

			UInt move_count = Count - index - 1;
			UInt move_size = move_count * sizeof(T);

			Splice<T> tmp = p.Allocate<T>(move_size);
			std::memcpy(tmp.Elements, & Buffer[index + 1], move_size);
			std::memcpy(& Buffer[index], tmp.Elements, move_size);

			return result;
		}

		T PopBack()
		{
			FE_CORE_ASSERT(Count, "Arena is empty!");
			Count--;
			T* result = Buffer.Elements + Count;
			return *result;
		}

		T SwapWithBackAndPop(UInt index)
		{
			FE_CORE_ASSERT(index < Count, "Index past occupied part of arena");

			std::swap(Buffer[index], Buffer[Count - 1]);
			return PopBack();
		}

		Splice<T> GetSplice()
		{
			Splice<T> result;
			result.Elements = Buffer.Elements;
			result.Count = Count;
			return result;
		}
	};

	template <typename T>
	struct SpliceArena
	{
		UInt Count;
		Splice<T> Buffer;

		FE_TYPE_SETUP(SpliceArena<T>)

		const	T* begin()	const	{ return Buffer.Elements; }
				T* begin()			{ return Buffer.Elements; }
		const	T* end()	const	{ return Buffer.Elements + Count; }
				T* end()			{ return Buffer.Elements + Count; }

		bool IsFull() const { return Count >= Buffer.Count; }

		void Init()
		{
			Buffer.Init();
			Count = 0;
		}

		void Init(Splice<T> splice, UInt count = 0)
		{
			Buffer = splice;
			Count = count;
		}

		void InitAllocate(UInt capacity, Allocator* alloc = Context::Alloc::Default)
		{
			FE_CORE_ASSERT(!Buffer.Elements, "Not released SpliceArena - memory leak!");
			((PMAlloc*)alloc)->Allocate<T>(capacity);
			Count = 0;
		}

		void Clear()
		{
			Count = 0;
		}

		void Release(Allocator* alloc = Context::Alloc::Default)
		{
			((PMAlloc*)alloc)->Deallocate(Buffer);
			Buffer.Elements = nullptr;
			Count = 0;
		}

		T& operator[](UInt i)
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Buffer[i];
		}

		T& At(UInt i)
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Buffer[i];
		}

		const T& operator[](UInt i) const
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Buffer[i];
		}

		const T& At(UInt i) const
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Buffer[i];
		}

		void Insert(const T& data, UInt index)
		{
			FE_CORE_ASSERT(Count > index, "Index past occupied part of arena");
			FE_CORE_ASSERT(Count < Buffer.Count, "Arena overflow!");

			Pile p;

			UInt move_count = Count - index;
			UInt move_size = move_count * sizeof(T);

			Splice<T> tmp = p.Allocate<T>(move_count);

			std::memcpy(tmp.Elements, & Buffer[index], move_size);
			std::memcpy(& Buffer[index + 1], tmp.Elements, move_size);
			Buffer[index] = data;

			++Count;
		}

		T* Emplace(UInt index)
		{
			FE_CORE_ASSERT(Count > index, "Index past occupied part of arena");
			FE_CORE_ASSERT(Count < Buffer.Count, "Arena overflow!");

			Pile p;

			UInt move_count = Count - index;
			UInt move_size = move_count * sizeof(T);

			Splice<T> tmp = p.Allocate<T>(move_count);

			std::memcpy(tmp.Elements, & Buffer[index], move_size);
			std::memcpy(& Buffer[index + 1], tmp.Elements, move_size);

			++Count;
			return &Buffer[index];
		}

		void Append(const T& data)
		{
			FE_CORE_ASSERT(Count < Buffer.Count, "Arena overflow!");
			Buffer[Count] = data;
			Count++;
		}

		void Append(Splice<T> splice)
		{
			FE_CORE_ASSERT(Count + splice.Count < Buffer.Count, "Arena overflow!");
			FE_CORE_ASSERT(splice.Elements, "Appending invalid splice to SpliceArena");
			std::memcpy(&Buffer[Count], splice.begin(), splice.Count);
			Count += splice.Count;
		}

		T* EmplaceBack()
		{
			FE_CORE_ASSERT(Count < Buffer.Count, "Arena overflow!");
			T* result = Buffer.Elements + Count;
			Count++;
			return result;
		}

		T Pop(UInt index)
		{
			FE_CORE_ASSERT(index < Count, "Index past occupied part of arena");

			T result = Buffer[index];

			Pile p;

			UInt move_count = Count - index - 1;
			UInt move_size = move_count * sizeof(T);

			Splice<T> tmp = p.Allocate<T>(move_size);
			std::memcpy(tmp.Elements, & Buffer[index + 1], move_size);
			std::memcpy(& Buffer[index], tmp.Elements, move_size);

			return result;
		}

		T PopBack()
		{
			FE_CORE_ASSERT(Count, "Arena is empty!");
			Count--;
			T* result = Buffer.Elements + Count;
			return *result;
		}

		T SwapWithBackAndPop(UInt index)
		{
			FE_CORE_ASSERT(index < Count, "Index past occupied part of arena");

			std::swap(Buffer[index], Buffer[Count - 1]);
			return PopBack();
		}

		Splice<T> GetSplice()
		{
			Splice<T> result;
			result.Elements = Buffer.Elements;
			result.Count = Count;
			return result;
		}
	};

	template <typename T>
	struct DynamicArena
	{
		PMAlloc* Alloc;
		UInt Count;
		Splice<T> Buffer;

		FE_TYPE_SETUP(DynamicArena<T>)

		const	T* begin() const { return Buffer.Elements; }
				T* begin()       { return Buffer.Elements; }
		const	T* end() const { return Buffer.Elements + Count; }
				T* end()       { return Buffer.Elements + Count; }

		bool IsFull() const { return Count == Buffer.Count; }

		void Init(Allocator* allocator = Context::Alloc::Default)
		{
			Alloc = (PMAlloc*) allocator;
			Count = 0;
			Buffer.Init();
		}

		void Clear()
		{
			Count = 0;
		}

		void Release()
		{
			Alloc->Deallocate(Buffer);
			Count = 0;
			Buffer.Init();
		}

		T& operator[](UInt i)
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Buffer[i];
		}

		T& At(UInt i)
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Buffer[i];
		}

		const T& operator[](UInt i) const
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Buffer[i];
		}

		const T& At(UInt i) const
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Buffer[i];
		}

		void Insert(const T& data, UInt index)
		{
			FE_CORE_ASSERT(Count > index);

			FE_LOG_CORE_WARN("Could be optimized!"); // by combining ExpandDefault() and move to make place for new element

			if (Count == Buffer.Count)
			{
				ExpandDefault();
			}

			Pile p;

			UInt move_count = Count - index;
			UInt move_size = move_count * sizeof(T);

			Splice<T> tmp = p.Allocate<T>(move_count);

			std::memcpy(tmp.Elements, & Buffer[index], move_size);
			Buffer[index] = data;
			std::memcpy(& Buffer[index+1], tmp.Elements, move_size);

			++Count;
		}

		T* Emplace(UInt index)
		{
			FE_CORE_ASSERT(Count > index);

			FE_LOG_CORE_WARN("Could be optimized!"); // by combining ExpandDefault() and move to make place for new element

			if (Count == Buffer.Count)
			{
				ExpandDefault();
			}

			Pile p;

			UInt move_count = Count - index;
			UInt move_size = move_count * sizeof(T);

			Splice<T> tmp = p.Allocate<T>(move_count);

			std::memcpy(tmp.Elements, & Buffer[index], move_size);
			std::memcpy(& Buffer[index + 1], tmp.Elements, move_size);

			++Count;
			return &Buffer[index];
		}

		void Append(const T& data)
		{
			if (Count == Buffer.Count)
			{
				ExpandDefault();
			}

			Buffer[Count] = data;
			Count++;
		}

		T* EmplaceBack()
		{
			if (Count == Buffer.Count)
			{
				ExpandDefault();
			}

			T* result = Buffer.Elements + Count;
			Count++;
			return result;
		}

		T PopBack()
		{
			FE_CORE_ASSERT(Count, "Arena is empty!");
			Count--;
			T* result = Buffer.Elements + Count;
			return *result;
		}

		T SwapWithBackAndPop(UInt index)
		{
			FE_CORE_ASSERT(Count, "Arena is empty!");
			std::swap(Buffer[index], Buffer[Count - 1]);
			return PopBack();
		}

		Splice<T> GetSplice()
		{
			Splice<T> result;
			result.Elements = Buffer.Elements;
			result.Count = Count;
			return result;
		}

		void ReserveExact(UInt capacity)
		{
			FE_CORE_ASSERT(capacity > Buffer.Count, "DynamicArena allready bigger");

			AllocateAndMove(capacity);
		}

		void ReserveAtLeast(UInt capacity)
		{
			FE_CORE_ASSERT(capacity > Buffer.Count, "DynamicArena allready bigger");

			bool any_capacity = Buffer.Count;
			UInt new_capacity = Buffer.Count + (Buffer.Count >> 1); // *1.5
			new_capacity = new_capacity * any_capacity + 2 * !any_capacity;

			bool default_better = capacity < new_capacity;
			new_capacity = new_capacity * default_better + capacity * !default_better;

			AllocateAndMove(new_capacity);
		}

		void ExpandDefault()
		{
			bool any_capacity = Buffer.Count;
			UInt new_capacity = Buffer.Count + (Buffer.Count >> 1); // *1.5
			new_capacity = new_capacity * any_capacity + 2 * !any_capacity;

			AllocateAndMove(new_capacity);
		}

		void AllocateAndMove(UInt capacity)
		{
			Splice<T> move_splice = Buffer;
			move_splice.Count = Count;

			Splice<T> new_buffer = Alloc->Reallocate<T>(Buffer, move_splice, capacity);

			FE_CORE_ASSERT(new_buffer.Elements && new_buffer.Count, "DynamicArena realloc failed");

			Buffer = new_buffer;
		}
	};

	template <typename T, typename tnAlloc>
	struct DynamicArenaSA : public DynamicArena<T>
	{
		FE_TYPE_SETUP(DynamicArenaSA<T, tnAlloc>)

		void Release()
		{
			DynamicArena<T>::Alloc->Cast<TypedAlloc<tnAlloc>>().Deallocate(DynamicArena<T>::Buffer);
			DynamicArena<T>::Count = 0;
			DynamicArena<T>::Buffer.Init();
		}

		void Insert(const T& data, UInt index)
		{
			FE_CORE_ASSERT(DynamicArena<T>::Count > index);

			FE_LOG_CORE_WARN("Could be optimized!"); // by combining ExpandDefault() and move to make place for new element

			if (DynamicArena<T>::Count == DynamicArena<T>::Buffer.Count)
			{
				ExpandDefault();
			}

			Pile p;

			UInt move_count = DynamicArena<T>::Count - index;
			UInt move_size = move_count * sizeof(T);

			Splice<T> tmp = p.Allocate<T>(move_count);

			std::memcpy(tmp.Elements, &(DynamicArena<T>::Buffer[index]), move_size);
			DynamicArena<T>::Buffer[index] = data;
			std::memcpy(&(DynamicArena<T>::Buffer[index + 1]), tmp.Elements, move_size);

			++DynamicArena<T>::Count;
		}

		T* Emplace(UInt index)
		{
			FE_CORE_ASSERT(DynamicArena<T>::Count > index);

			FE_LOG_CORE_WARN("Could be optimized!"); // by combining ExpandDefault() and move to make place for new element

			if (DynamicArena<T>::Count == DynamicArena<T>::Buffer.Count)
			{
				ExpandDefault();
			}

			Pile p;

			UInt move_count = DynamicArena<T>::Count - index;
			UInt move_size = move_count * sizeof(T);

			Splice<T> tmp = p.Allocate<T>(move_count);

			std::memcpy(tmp.Elements, &(DynamicArena<T>::Buffer[index]), move_size);
			std::memcpy(&(DynamicArena<T>::Buffer[index + 1]), tmp.Elements, move_size);

			++DynamicArena<T>::Count;
			return &(DynamicArena<T>::Buffer[index]);
		}

		void Append(const T& data)
		{
			if (DynamicArena<T>::Count == DynamicArena<T>::Buffer.Count)
			{
				ExpandDefault();
			}

			DynamicArena<T>::Buffer[DynamicArena<T>::Count] = data;
			DynamicArena<T>::Count++;
		}

		T* EmplaceBack()
		{
			if (DynamicArena<T>::Count == DynamicArena<T>::Buffer.Count)
			{
				ExpandDefault();
			}

			T* result = DynamicArena<T>::Buffer.Elements + DynamicArena<T>::Count;
			DynamicArena<T>::Count++;
			return result;
		}

		void ReserveExact(UInt capacity)
		{
			FE_CORE_ASSERT(capacity > DynamicArena<T>::Buffer.Count, "DynamicArenaSA allready bigger");

			AllocateAndMove(capacity);
		}

		void ReserveAtLeast(UInt capacity)
		{
			FE_CORE_ASSERT(capacity > DynamicArena<T>::Buffer.Count, "DynamicArenaSA allready bigger");

			bool any_capacity = DynamicArena<T>::Buffer.Count;
			UInt new_capacity = DynamicArena<T>::Buffer.Count + (DynamicArena<T>::Buffer.Count >> 1); // *1.5
			new_capacity = new_capacity * any_capacity + 2 * !any_capacity;

			bool default_better = capacity < new_capacity;
			new_capacity = new_capacity * default_better + capacity * !default_better;

			AllocateAndMove(new_capacity);
		}

		void ExpandDefault()
		{
			bool any_capacity = DynamicArena<T>::Buffer.Count;
			UInt new_capacity = DynamicArena<T>::Buffer.Count + (DynamicArena<T>::Buffer.Count >> 1); // *1.5
			new_capacity = new_capacity * any_capacity + 2 * !any_capacity;

			AllocateAndMove(new_capacity);
		}

		void AllocateAndMove(UInt capacity)
		{
			Splice<T> move_splice = DynamicArena<T>::Buffer;
			move_splice.Count = DynamicArena<T>::Count;

			Splice<T> new_buffer = DynamicArena<T>::Alloc->Cast<TypedAlloc<tnAlloc>>().Reallocate<T>(DynamicArena<T>::Buffer, move_splice, capacity);

			FE_CORE_ASSERT(new_buffer.Elements && new_buffer.Count, "DynamicArena realloc failed");

			DynamicArena<T>::Buffer = new_buffer;
		}
	};
}