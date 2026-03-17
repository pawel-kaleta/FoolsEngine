#pragma once

#include "FoolsEngine/Foundation/Utils/BitOperations.h"
#include "FoolsEngine/Foundation/Debug/Asserts.h"

#include <vector>
#include <memory_resource>
#include <memory>

namespace fe
{
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
}