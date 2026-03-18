#pragma once

#include "DataTypes.h"
#include "Allocator.h"
#include "FoolsEngine/Foundation/Debug/Asserts.h"

namespace fe
{
	template <typename tn>
	struct Splice
	{
		tn* Elements = nullptr;
		UInt Count = 0;

		tn& operator[](UInt i)
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Elements[i];
		}
	};

	template <typename tn>
	class Array : public Splice<tn>
	{
	public:
		UInt Capacity = 0;

		void InitArray(tn* elements, UInt capacity)
		{
			Splice<tn>::Elements = elements;
			Capacity = capacity;
			Splice<tn>::Count = 0;
		}

		tn& operator[](UInt i)
		{
			FE_CORE_ASSERT(i < Capacity, "Out of Splice bound!");
			return Splice<tn>::Elements[i];
		}

		tn& Append(const tn& element)
		{
			FE_CORE_ASSERT(Splice<tn>::Count < Capacity, "Out of Array bounds!");
			Splice<tn>::Elements[Splice<tn>::Count] = element;
			tn& result = Splice<tn>::Elements[Splice<tn>::Count];
			++Splice<tn>::Count;
			return result;
		}

		tn& PushBack()
		{
			FE_CORE_ASSERT(Splice<tn>::Count < Capacity, "Out of Array bounds!");
			tn& result = Splice<tn>::Elements[Splice<tn>::Count];
			++Splice<tn>::Count;
			return result;
		}

		void PopBack()
		{
			FE_CORE_ASSERT(Splice<tn>::Count, "Array is empty!");
			Splice<tn>::Count--;
		}
	};

	// dont instantiate, use DynArrStat instead
	template <typename tn>
	class DynArr : public Array<tn>
	{
	public:
		Allocator* Alloc = nullptr;

		tn& Append(const tn& element)
		{
			if (Array<tn>::Count == Array<tn>::Capacity)
				ResizeAndRelocate();

			Array<tn>::Elements[Array<tn>::Count] = element;
			tn& result = Array<tn>::Elements[Array<tn>::Count];
			++Array<tn>::Count;
			return result;
		}

		tn& PushBack()
		{
			if (Array<tn>::Count == Array<tn>::Capacity)
				ResizeAndRelocate();

			tn& result = Array<tn>::Elements[Array<tn>::Count];
			++Array<tn>::Count;
			return result;
		}

		void ResizeAndRelocate()
		{
			UInt new_capacity;
			if (Array<tn>::Capacity)
				new_capacity = Array<tn>::Capacity + Array<tn>::Capacity / 2; // *1.5
			else
				new_capacity = 4;
			MemReg new_elements = Alloc->Allocate<tn>(new_capacity);

			if (Array<tn>::Elements)
			{
				UInt old_size = Array<tn>::Capacity * sizeof(tn);
				std::memcpy(new_elements.Data, Array<tn>::Elements, old_size);
				Alloc->Deallocate({ Array<tn>::Elements, old_size });
			}

			Array<tn>::Elements = new_elements.Data;
			Array<tn>::Capacity = new_capacity;
		}

		void ReserveExact(UInt capacity)
		{
			if (capacity <= Array<tn>::Capacity)
			{
				FE_LOG_CORE_WARN("Attempt to reserve DynArr capacity to no more then it already have!");
				return;
			}

			MemReg new_elements = Alloc->Allocate<tn>(capacity);

			if (Array<tn>::Elements)
			{
				UInt old_size = Array<tn>::Capacity * sizeof(tn);
				std::memcpy(new_elements.Data, Array<tn>::Elements, old_size);
				Alloc->Deallocate({ Array<tn>::Elements, old_size });
			}

			Array<tn>::Elements = new_elements.Data;
			Array<tn>::Capacity = capacity;
		}

		void ReserveAtLeast(UInt capacity)
		{
			if (capacity <= Array<tn>::Capacity)
			{
				FE_LOG_CORE_WARN("Attempt to reserve DynArr capacity to no more then it already have!");
				return;
			}

			UInt new_capacity;
			if (Array<tn>::Capacity)
				new_capacity = Array<tn>::Capacity + Array<tn>::Capacity / 2; // *1.5
			else
				new_capacity = 4;

			if (new_capacity < capacity)
				new_capacity = capacity;

			MemReg new_elements = Alloc->Allocate<tn>(new_capacity);

			if (Array<tn>::Elements)
			{
				UInt old_size = Array<tn>::Capacity * sizeof(tn);
				std::memcpy(new_elements.Data, Array<tn>::Elements, old_size);
				Alloc->Deallocate({ Array<tn>::Elements, old_size });
			}

			Array<tn>::Elements = new_elements.Data;
			Array<tn>::Capacity = new_capacity;
		}
	};

	template <typename tn, class tAlloc>
	class DynArrStat : public DynArr<tn>
	{
	public:
		void InitDynArrStat(tAlloc* allocator) { DynArr<tn>::Alloc = allocator; }

		tn& Append(const tn& element)
		{
			if (DynArr<tn>::Count == DynArr<tn>::Capacity)
				ResizeAndRelocate();

			DynArr<tn>::Elements[DynArr<tn>::Count] = element;
			tn& result = DynArr<tn>::Elements[DynArr<tn>::Count];
			++DynArr<tn>::Count;
			return result;
		}

		tn& PushBack()
		{
			if (DynArr<tn>::Count == DynArr<tn>::Capacity)
				ResizeAndRelocate();

			tn& result = DynArr<tn>::Elements[DynArr<tn>::Count];
			++DynArr<tn>::Count;
			return result;
		}

		void ResizeAndRelocate()
		{
			UInt new_capacity;
			if (Array<tn>::Capacity)
				new_capacity = Array<tn>::Capacity + Array<tn>::Capacity / 2; // *1.5
			else
				new_capacity = 4;
			MemReg new_elements = ((tAlloc*)DynArr<tn>::Alloc)->Allocate<tn>(new_capacity);

			if (Array<tn>::Elements)
			{
				UInt old_size = Array<tn>::Capacity * sizeof(tn);
				std::memcpy(new_elements.Data, Array<tn>::Elements, old_size);
				((tAlloc*)DynArr<tn>::Alloc)->Deallocate({ Array<tn>::Elements, old_size });
			}

			DynArr<tn>::Elements = new_elements.Data;
			DynArr<tn>::Capacity = new_capacity;
		}

		void ReserveExact(UInt capacity)
		{
			if (capacity <= Array<tn>::Capacity)
			{
				FE_LOG_CORE_WARN("Attempt to reserve DynArr capacity to no more then it already have!");
				return;
			}

			MemReg new_elements = ((tAlloc*)DynArr<tn>::Alloc)->Allocate<tn>(capacity);

			if (Array<tn>::Elements)
			{
				UInt old_size = Array<tn>::Capacity * sizeof(tn);
				std::memcpy(new_elements.Data, Array<tn>::Elements, old_size);
				((tAlloc*)DynArr<tn>::Alloc)->Deallocate({ Array<tn>::Elements, old_size });
			}

			Array<tn>::Elements = new_elements.Data;
			Array<tn>::Capacity = capacity;
		}

		void ReserveAtLeast(UInt capacity)
		{
			if (capacity <= Array<tn>::Capacity)
			{
				FE_LOG_CORE_WARN("Attempt to reserve DynArr capacity to no more then it already have!");
				return;
			}

			UInt new_capacity;
			if (Array<tn>::Capacity)
				new_capacity = Array<tn>::Capacity + Array<tn>::Capacity / 2; // *1.5
			else
				new_capacity = 4;

			if (new_capacity < capacity)
				new_capacity = capacity;

			MemReg new_elements = ((tAlloc*)DynArr<tn>::Alloc)->Allocate<tn>(new_capacity);

			if (Array<tn>::Elements)
			{
				UInt old_size = Array<tn>::Capacity * sizeof(tn);
				std::memcpy(new_elements.Data, Array<tn>::Elements, old_size);
				((tAlloc*)DynArr<tn>::Alloc)->Deallocate({ Array<tn>::Elements, old_size });
			}

			Array<tn>::Elements = new_elements.Data;
			Array<tn>::Capacity = new_capacity;
		}
	};
}