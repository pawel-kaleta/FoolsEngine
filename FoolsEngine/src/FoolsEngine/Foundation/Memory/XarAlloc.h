#pragma once

namespace fe
{
	// this is for function parameters only
	// don't instantiate, use XarrAlloc or MakeXarr<T>(alloc) instead
	template <typename T>
	class XarrAllocPM
	{
	public:
		UInt Count = 0;
		Allocator* AllocMain = nullptr;
		Allocator* AllocAux = nullptr;
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
				UInt size = 1;
				for (UInt i = 0; i < Chunks.Count; ++i)
				{
					Splice<T> region;
					region.Elements = Chunks[i];
					region.Count = size;
					AllocMain->Deallocate(region);
					size = size << 1;
				}

				AllocAux->Deallocate(Chunks.Buffer);
			}

			Count = 0;
			Chunks.Count = 0;
			Chunks.Buffer.Count = 0;
			Chunks.Buffer.Elements = nullptr;
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

		void Append(const T* t)
		{
			if (Count == Capacity())
				Expand();

			Count++;

			unsigned long chunk_i;
			MSB64(&chunk_i, Count);
			auto chunk_mask = (U64)1 << chunk_i;
			auto in_chunk_i = Count - chunk_mask;
			auto result_ptr = Chunks[chunk_i] + in_chunk_i;

			*result_ptr = t;
		}

		T* PushBack()
		{
			if (Count == Capacity())
				Expand();

			Count++;

			unsigned long chunk_i;
			MSB64(&chunk_i, Count);
			auto chunk_mask = (U64)1 << chunk_i;
			auto in_chunk_i = Count - chunk_mask;
			auto result_ptr = Chunks[chunk_i] + in_chunk_i;

			return result_ptr;
		}

		T PopBack()
		{
			unsigned long chunk_i;
			MSB64(&chunk_i, Count);
			auto chunk_mask = (U64)1 << chunk_i;
			auto in_chunk_i = Count - chunk_mask;
			auto result_ptr = Chunks[chunk_i] + in_chunk_i;

			Count--;

			return *result_ptr;
		}

		template <class tnAllocator>
		Splice<T> GetCopyContiguous(tnAllocator* alloc)
		{
			Splice<T> result = alloc->Allocate<T>(Count);

			Byte* current = (Byte*)result.Elements;
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
			if (Chunks.Count == Chunks.Buffer.Count)
			{
				bool any_capacity = Chunks.Buffer.Count;
				UInt chunks_new_capacity = Chunks.Buffer.Count + (Chunks.Buffer.Count >> 1); // *1.5
				chunks_new_capacity = chunks_new_capacity * any_capacity + (UInt)2 * !any_capacity;

				Splice<T*> new_chunks = AllocAux->Allocate<T*>(chunks_new_capacity);

				UInt old_size = Chunks.Buffer.Count * sizeof(T*);
				std::memcpy(new_chunks.Elements, Chunks.Buffer.Elements, old_size);
				AllocAux->Deallocate(Chunks.Buffer);

				Chunks.Buffer = new_chunks;
			}

			UInt new_chunk_capacity = UInt(1) << Chunks.Count;
			Splice<T> mem_reg = AllocMain->Allocate<T>(new_chunk_capacity);
			Chunks.Append(mem_reg.Elements);
		}
	};

	template <typename T, class tnAllocMain, class tnAllocAux>
	class XarrAlloc final : public XarrAllocPM<T>
	{
	public:
		void InitXarrAlloc(tnAllocMain* allocMain, tnAllocAux* allocAux)
		{
			Xarr<T>::AllocMain = allocMain;
			Xarr<T>::AllocAux = allocAux;
		}

		void Release()
		{
			if (XarrAllocPM<T>::Chunks.Count)
			{
				UInt size = 1;
				for (UInt i = 0; i < XarrAllocPM<T>::Chunks.Count; ++i)
				{
					Splice<T> region;
					region.Elements = XarrAllocPM<T>::Chunks[i];
					region.Count = size;
					((tnAllocMain*)XarrAllocPM<T>::AllocMain)->Deallocate(region);
					size = size << 1;
				}

				((tnAllocMain*)XarrAllocPM<T>::AllocAux)->Deallocate(XarrAllocPM<T>::Chunks.Buffer);
			}

			XarrAllocPM<T>::Count = 0;
			XarrAllocPM<T>::Chunks.Count = 0;
			XarrAllocPM<T>::Chunks.Buffer.Count = 0;
			XarrAllocPM<T>::Chunks.Buffer.Elements = nullptr;
		}

		void Append(const T* t)
		{
			if (XarrAllocPM<T>::Count == XarrAllocPM<T>::Capacity())
				Expand();

			XarrAllocPM<T>::Count++;

			unsigned long chunk_i;
			MSB64(&chunk_i, XarrAllocPM<T>::Count);
			auto chunk_mask = (XarrAllocPM)1 << chunk_i;
			auto in_chunk_i = XarrAllocPM<T>::Count - chunk_mask;
			auto result_ptr = XarrAllocPM<T>::Chunks[chunk_i] + in_chunk_i;

			*result_ptr = *t;
		}

		T* PushBack()
		{
			if (XarrAllocPM<T>::Count == XarrAllocPM<T>::Capacity())
				Expand();

			XarrAllocPM<T>::Count++;

			unsigned long chunk_i;
			MSB64(&chunk_i, XarrAllocPM<T>::Count);
			auto chunk_mask = (U64)1 << chunk_i;
			auto in_chunk_i = XarrAllocPM<T>::Count - chunk_mask;
			auto result_ptr = XarrAllocPM<T>::Chunks[chunk_i] + in_chunk_i;

			return result_ptr;
		}

	private:
		void Expand()
		{
			if (XarrAllocPM<T>::Chunks.Count == XarrAllocPM<T>::Chunks.Buffer.Count)
			{
				bool any_capacity = XarrAllocPM<T>::Chunks.Buffer.Count;
				UInt chunks_new_capacity = XarrAllocPM<T>::Chunks.Buffer.Count + (XarrAllocPM<T>::Chunks.Buffer.Count >> 1); // *1.5
				chunks_new_capacity = chunks_new_capacity * any_capacity + (UInt)2 * !any_capacity;

				Splice<T*> new_chunks = ((tnAllocAux*)XarrAllocPM<T>::AllocAux)->Allocate<T*>(chunks_new_capacity);

				UInt old_size = XarrAllocPM<T>::Chunks.Buffer.Count * sizeof(T*);
				std::memcpy(new_chunks.Elements, XarrAllocPM<T>::Chunks.Buffer.Elements, old_size);
				((tnAllocAux*)XarrAllocPM<T>::AllocAux)->Deallocate(Chunks.Buffer);

				XarrAllocPM<T>::Chunks.Buffer = new_chunks;
			}

			UInt new_chunk_capacity = UInt(1) << XarrAllocPM<T>::Chunks.Count;
			Splice<T> mem_reg = ((tnAllocMain*)XarrAllocPM<T>::AllocMain)->Allocate<T>(new_chunk_capacity);
			XarrAllocPM<T>::Chunks.Append(mem_reg.Elements);
		}
	};
}