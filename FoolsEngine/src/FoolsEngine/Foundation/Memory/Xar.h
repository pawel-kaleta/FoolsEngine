#pragma once

#include "Allocator.h"
#include "Array.h"

#include "FoolsEngine/Foundation/Utils/BitOperations.h"
#include "FoolsEngine/Foundation/Common.h"

#include <vector>
#include <memory_resource>
#include <memory>

namespace fe
{
	// to be removed!
	template <typename T>
	class Xar
	{
	public:
		Xar(std::pmr::polymorphic_allocator<std::byte> alloc) :
			m_Size(0), m_Chunks(alloc) { }

		Xar()
			: m_Size(0) { }

		~Xar()
		{
			auto alloc = m_Chunks.get_allocator();

			size_t chunk_size = 1;
			for (size_t i = 0; i < m_Chunks.size(); i++)
			{
				T* t = m_Chunks[i];
				for (size_t j = 0; j < chunk_size; j++)
				{
					t->~T();
					t++;
				}
				alloc.deallocate_object<T>(t, chunk_size);
				chunk_size <<= 1;
			}
		}

		T& operator[](size_t i)
		{
			FE_CORE_ASSERT(i < m_Size, "Out of bounds.");
			
			i++;

			unsigned long chunk;
			MSB64(&chunk, i);
			auto chunk_mask = (uint64_t)1 << chunk;
			auto in_chunk_i = i - chunk_mask;
			auto result_ptr = (T*)(m_Chunks[chunk]) + in_chunk_i;
			return *result_ptr;
		}

		void PushBack(T&& t)
		{
			size_t i = m_Size + 1;
			unsigned long chunk;
			MSB64(&chunk, i);
			if (chunk == m_Chunks.size())
			{
				m_Chunks.push_back(m_Chunks.get_allocator().allocate_object<T>(1<<chunk));
			}
			auto chunk_mask = (uint64_t)1 << chunk;
			auto in_chunk_i = i - chunk_mask;

			auto result_ptr = m_Chunks[chunk] + in_chunk_i;
			*result_ptr = t;

			m_Size++;
		}

		void PushBack(const T& t)
		{
			size_t i = m_Size + 1;
			unsigned long chunk;
			MSB64(&chunk, i);
			if (chunk == m_Chunks.size())
			{
				m_Chunks.push_back(m_Chunks.get_allocator().allocate_object<T>(1 << chunk));
			}
			auto chunk_mask = (uint64_t)1 << chunk;
			auto in_chunk_i = i - chunk_mask;

			auto result_ptr = m_Chunks[chunk] + in_chunk_i;
			*result_ptr = t;

			m_Size++;
		}

		T&& PopBack()
		{
			FE_CORE_ASSERT(0 < m_Size, "Poping from empty Xar");
			size_t i = m_Size;
			unsigned long chunk;
			MSB64(&chunk, i);
			auto chunk_mask = (uint64_t)1 << chunk;
			auto in_chunk_i = i - chunk_mask;

			auto result_ptr = m_Chunks[chunk] + in_chunk_i;

			m_Size--;

			return std::move(*result_ptr); // is this correct?
			//((T*)result_ptr)->~T();
		}

		template <typename... Args>
		T& EmplaceBack(Args&&... args)
		{
			size_t i = m_Size + 1;
			unsigned long chunk;
			MSB64(&chunk, i);
			if (chunk == m_Chunks.size())
			{
				m_Chunks.push_back(m_Chunks.get_allocator().allocate_object<T>(1 << chunk));
			}
			auto chunk_mask = (uint64_t)1 << chunk;
			auto in_chunk_i = i - chunk_mask;

			auto result_ptr = m_Chunks[chunk] + in_chunk_i;
			new (result_ptr) T(std::forward<Args>(args)...);

			m_Size++;

			return *result_ptr;
		}

		size_t Size() { return m_Size; }
	private:
		std::pmr::vector<T*> m_Chunks;
		uint64_t m_Size;
	};

	template <typename T>
	class Xarr
	{
	public:
		UInt Count = 0;
		Array<T*> Chunks;

		UInt Capacity()
		{
			U64 result = -1;
			// shift by 64 as 1 op is UB and sets result to -1
			result = result >> (63 - Chunks.Count);
			result = result >> 1;

			return result;
		}

		void Release()
		{
			if (Chunks.Count)
			{
				UInt size = sizeof(T);
				for (UInt i = 0; i < Chunks.Count; ++i)
				{
					MemReg region;
					region.Data = (Byte*)(Chunks[i]);
					region.Size = size;
					Alloc->Deallocate(region);
					size = size << 1;
				}

				MemReg region;
				region.Data = (Byte*)Chunks.Elements;
				region.Size = Chunks.Capacity * sizeof(T*);
				Alloc->Deallocate(region);
			}

			Count = 0;
			Chunks.Capacity = 0;
			Chunks.Count = 0;
			Chunks.Elements = nullptr;
		}

		void Deinit()
		{
			Release();
			Alloc = nullptr;
		}

		T& operator[](UInt i)
		{
			++i;

			unsigned long chunk_i;
			MSB64(&chunk_i, i);
			UInt chunk_mask = UInt(1) << chunk_i;
			UInt in_chunk_i = i - chunk_mask;
			T* result_ptr = Chunks[chunk_i] + in_chunk_i;
			return *result_ptr;
		}

		const T& operator[](UInt i) const
		{
			++i;

			unsigned long chunk_i;
			MSB64(&chunk_i, i);
			UInt chunk_mask = UInt(1) << chunk_i;
			UInt in_chunk_i = i - chunk_mask;
			T* result_ptr = (Chunks[chunk_i]) + in_chunk_i;
			return *result_ptr;
		}

		void Append(const T& t)
		{
			if (Count == Capacity())
				Expand();

			++Count;

			unsigned long chunk_i;
			MSB64(&chunk_i, Count);
			auto chunk_mask = (U64)1 << chunk_i;
			auto in_chunk_i = Count - chunk_mask;
			auto result_ptr = Chunks[chunk_i] + in_chunk_i;

			*result_ptr = t;
		}

		T& PushBack()
		{
			if (Count == Capacity())
				Expand();

			++Count;

			unsigned long chunk_i;
			MSB64(&chunk_i, Count);
			auto chunk_mask = (U64)1 << chunk_i;
			auto in_chunk_i = Count - chunk_mask;
			auto result_ptr = Chunks[chunk_i] + in_chunk_i;

			new (result_ptr) T();
			return *result_ptr;
		}

		T PopBack()
		{
			unsigned long chunk_i;
			MSB64(&chunk_i, Count);
			auto chunk_mask = (U64)1 << chunk_i;
			auto in_chunk_i = Count - chunk_mask;
			auto result_ptr = Chunks[chunk_i] + in_chunk_i;

			--Count;

			return *result_ptr;
		}

		template <class tnAllocator>
		Splice<T> GetCopyContiguous(tnAllocator& alloc)
		{
			Splice<T> result;
			result.Allocate(alloc, Count);
			T* current = result.Elements;

			UInt chunk_legth = 1;
			UInt last_chunk = Chunks.Count - 1;
			for (UInt i = 0; i < last_chunk; ++i)
			{
				UInt chunk_size = chunk_legth * sizeof(T);
				std::memcpy(current, Chunks[i], chunk_size);
				current += chunk_legth;
				chunk_legth = chunk_legth << 1;
			}

			auto in_chunk_i = Count - chunk_legth;
			auto last_chunk_used_size = (in_chunk_i + 1) * sizeof(T);
			std::memcpy(current, Chunks[last_chunk], last_chunk_used_size);

			return result;
		}
	private:
		void Expand()
		{
			if (Chunks.Count == Chunks.Capacity)
			{
				bool any_capacity = Chunks.Capacity;
				UInt chunks_new_capacity = Chunks.Capacity + (Chunks.Capacity >> 1); // *1.5
				chunks_new_capacity = chunks_new_capacity * any_capacity + (UInt)2 * !any_capacity;

				auto& alloc = Context::Allocators::System::GeneralPurpose;
				MemReg new_elements = alloc->Allocate<T*>(chunks_new_capacity);

				UInt old_size = Chunks.Capacity * sizeof(T*);
				std::memcpy(new_elements.Data, Chunks.Elements, old_size);
				MemReg todealoc;
				todealoc.Data = (Byte*)Chunks.Elements;
				todealoc.Size = old_size;
				alloc->Deallocate(todealoc);

				Chunks.Elements = (T**)new_elements.Data;
				Chunks.Capacity = chunks_new_capacity;
			}

			UInt new_chunk_capacity = UInt(1) << Chunks.Count;
			MemReg mem_reg = Alloc->Allocate<T>(new_chunk_capacity);
			Chunks.Append((T*)mem_reg.Data);
		}

	};

	// this is for function parameters only
	// don't instantiate, use XarrAlloc or MakeXarr<T>(alloc) instead
	template <typename T>
	class XarrAllocPM
	{
	public:
		UInt Count = 0;
		Allocator* Alloc = nullptr;
		Array<T*> Chunks;

		UInt Capacity()
		{
			U64 result = -1;
			// shift by 64 as 1 op is UB and sets result to -1
			result = result >> (63 - Chunks.Count);
			result = result >> 1;

			return result;
		}

		void Release()
		{
			if (Chunks.Count)
			{
				UInt size = sizeof(T);
				for (UInt i = 0; i < Chunks.Count; ++i)
				{
					MemReg region;
					region.Data = (Byte*)(Chunks[i]);
					region.Size = size;
					Alloc->Deallocate(region);
					size = size << 1;
				}

				MemReg region;
				region.Data = (Byte*)Chunks.Elements;
				region.Size = Chunks.Capacity * sizeof(T*);
				Alloc->Deallocate(region);
			}

			Count = 0;
			Chunks.Capacity = 0;
			Chunks.Count = 0;
			Chunks.Elements = nullptr;
		}

		void Deinit()
		{
			Release();
			Alloc = nullptr;
		}

		T& operator[](UInt i)
		{
			++i;

			unsigned long chunk_i;
			MSB64(&chunk_i, i);
			UInt chunk_mask = UInt(1) << chunk_i;
			UInt in_chunk_i = i - chunk_mask;
			T* result_ptr = Chunks[chunk_i] + in_chunk_i;
			return *result_ptr;
		}

		const T& operator[](UInt i) const
		{
			++i;

			unsigned long chunk_i;
			MSB64(&chunk_i, i);
			UInt chunk_mask = UInt(1) << chunk_i;
			UInt in_chunk_i = i - chunk_mask;
			T* result_ptr = (Chunks[chunk_i]) + in_chunk_i;
			return *result_ptr;
		}

		void Append(const T& t)
		{
			if (Count == Capacity())
				Expand();

			++Count;

			unsigned long chunk_i;
			MSB64(&chunk_i, Count);
			auto chunk_mask = (U64)1 << chunk_i;
			auto in_chunk_i = Count - chunk_mask;
			auto result_ptr = Chunks[chunk_i] + in_chunk_i;

			*result_ptr = t;
		}

		T& PushBack()
		{
			if (Count == Capacity())
				Expand();

			++Count;

			unsigned long chunk_i;
			MSB64(&chunk_i, Count);
			auto chunk_mask = (U64)1 << chunk_i;
			auto in_chunk_i = Count - chunk_mask;
			auto result_ptr = Chunks[chunk_i] + in_chunk_i;

			new (result_ptr) T();
			return *result_ptr;
		}

		T PopBack()
		{
			unsigned long chunk_i;
			MSB64(&chunk_i, Count);
			auto chunk_mask = (U64)1 << chunk_i;
			auto in_chunk_i = Count - chunk_mask;
			auto result_ptr = Chunks[chunk_i] + in_chunk_i;

			--Count;

			return *result_ptr;
		}

		Splice<T> GetCopyContiguous(Pile& pile)
		{
			MemReg allocation = pile.Allocate<T>(Count); // allocate on local splice!
			Byte* current = allocation.Data;

			UInt chunk_legth = 1;
			UInt last_chunk = Chunks.Count - 1;
			for (UInt i = 0; i < last_chunk; ++i)
			{
				UInt chunk_size = chunk_legth * sizeof(T);
				std::memcpy(current, Chunks[i], chunk_size);
				current += chunk_size;
				chunk_legth = chunk_legth << 1;
			}

			auto in_chunk_i = Count - chunk_legth;
			auto last_chunk_used_size = (in_chunk_i + 1) * sizeof(T);
			std::memcpy(current, Chunks[last_chunk], last_chunk_used_size);

			FE_CORE_ASSERT(current + last_chunk_used_size == allocation.Data + allocation.Size, "Sanity check failed");

			return *(Splice<T>*) & allocation;
		}
	private:
		void Expand()
		{
			if (Chunks.Count == Chunks.Capacity)
			{
				bool any_capacity = Chunks.Capacity;
				UInt chunks_new_capacity = Chunks.Capacity + (Chunks.Capacity >> 1); // *1.5
				chunks_new_capacity = chunks_new_capacity * any_capacity + (UInt)2 * !any_capacity;

				auto& alloc = Context::Allocators::System::GeneralPurpose;
				MemReg new_elements = alloc->Allocate<T*>(chunks_new_capacity);

				UInt old_size = Chunks.Capacity * sizeof(T*);
				std::memcpy(new_elements.Data, Chunks.Elements, old_size);
				MemReg todealoc;
				todealoc.Data = (Byte*)Chunks.Elements;
				todealoc.Size = old_size;
				alloc->Deallocate(todealoc);

				Chunks.Elements = (T**)new_elements.Data;
				Chunks.Capacity = chunks_new_capacity;
			}

			UInt new_chunk_capacity = UInt(1) << Chunks.Count;
			MemReg mem_reg = Alloc->Allocate<T>(new_chunk_capacity);
			Chunks.Append((T*)mem_reg.Data);
		}
	};

	template <typename T, class tnAlloc>
	class XarrAlloc final : public Xarr<T>
	{
	public:
		XarrAlloc() { };
		XarrAlloc(tnAlloc* alloc) {	Xarr<T>::Alloc = alloc; }
		~XarrAlloc() { Release(); }

		void InitXarrAlloc(tnAlloc* alloc) { Xarr<T>::Alloc = alloc; }

		void Release()
		{
			if (Xarr<T>::Chunks.Count)
			{
				UInt size = sizeof(T);
				for (UInt i = 0; i < Xarr<T>::Chunks.Count; ++i)
				{
					MemReg region;
					region.Data = (Byte*)(Xarr<T>::Chunks[i]);
					region.Size = size;
					((tnAlloc*)Xarr<T>::Alloc)->Deallocate(region);
					size = size << 1;
				}

				MemReg region;
				region.Data = (Byte*)Xarr<T>::Chunks.Elements;
				region.Size = Xarr<T>::Chunks.Capacity * sizeof(T*);
				((tnAlloc*)Xarr<T>::Alloc)->Deallocate(region);
			}

			Xarr<T>::Count = 0;
			Xarr<T>::Chunks.Capacity = 0;
			Xarr<T>::Chunks.Count = 0;
			Xarr<T>::Chunks.Elements = nullptr;
		}

		void Deinit()
		{
			Release();
			Xarr<T>::Alloc = nullptr;
		}

		void Append(const T& t)
		{
			if (Xarr<T>::Count == Xarr<T>::Capacity())
				Expand();

			++Xarr<T>::Count;

			unsigned long chunk_i;
			MSB64(&chunk_i, Xarr<T>::Count);
			auto chunk_mask = (U64)1 << chunk_i;
			auto in_chunk_i = Xarr<T>::Count - chunk_mask;
			auto result_ptr = Xarr<T>::Chunks[chunk_i] + in_chunk_i;

			*result_ptr = t;
		}

		T& PushBack()
		{
			if (Xarr<T>::Count == Xarr<T>::Capacity())
				Expand();

			++Xarr<T>::Count;

			unsigned long chunk_i;
			MSB64(&chunk_i, Xarr<T>::Count);
			auto chunk_mask = (U64)1 << chunk_i;
			auto in_chunk_i = Xarr<T>::Count - chunk_mask;
			auto result_ptr = Xarr<T>::Chunks[chunk_i] + in_chunk_i;

			new (result_ptr) T();

			return *result_ptr;
		}

	private:
		void Expand()
		{
			if (Xarr<T>::Chunks.Count == Xarr<T>::Chunks.Capacity)
			{
				bool any_capacity = Xarr<T>::Chunks.Capacity;
				UInt chunks_new_capacity = Xarr<T>::Chunks.Capacity + (Xarr<T>::Chunks.Capacity >> 1); // *1.5
				chunks_new_capacity = chunks_new_capacity * any_capacity + (UInt)2 * !any_capacity;

				auto& alloc = Context::Allocators::System::GeneralPurpose;
				MemReg new_elements = alloc->Allocate<T*>(chunks_new_capacity);

				UInt old_size = Xarr<T>::Chunks.Capacity * sizeof(T*);
				std::memcpy(new_elements.Data, Xarr<T>::Chunks.Elements, old_size);
				MemReg todealoc;
				todealoc.Data = (Byte*)Xarr<T>::Chunks.Elements;
				todealoc.Size = old_size;
				alloc->Deallocate(todealoc);

				Xarr<T>::Chunks.Elements = (T**)new_elements.Data;
				Xarr<T>::Chunks.Capacity = chunks_new_capacity;
			}

			UInt new_chunk_capacity = UInt(1) << Xarr<T>::Chunks.Count;
			MemReg mem_reg = ((tnAlloc*)Xarr<T>::Alloc)->Allocate<T>(new_chunk_capacity);
			Xarr<T>::Chunks.Append((T*)mem_reg.Data);
		}

	};

	template <typename T, class alloc>
	inline XarrAlloc<T, alloc> MakeXarr(alloc* al) { return XarrAlloc<T, alloc>(al); }

	template <typename T>
	class XarrIt
	{
	public:
		Xarr<T>* m_Xarr = nullptr;
		T* ElementPtr = nullptr;
		void* PastChunk = nullptr;
		UInt ChunkIndex = -1;

		XarrIt(Xarr<T>* xarr)
		{
			m_Xarr = xarr;
			ElementPtr = xarr->Chunks[0];
			PastChunk = ElementPtr + 1;
			ChunkIndex = 0;
		}

		void operator++()
		{
			ElementPtr += 1;
			if (ElementPtr != PastChunk)
				return;

			++ChunkIndex;
			ElementPtr = m_Xarr->Chunks[ChunkIndex];

			UInt chunk_capacity = (UInt)1 << ChunkIndex;
			PastChunk = ElementPtr + chunk_capacity;
		}
	};
}