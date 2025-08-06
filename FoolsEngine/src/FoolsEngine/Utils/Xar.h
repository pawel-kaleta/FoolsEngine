#pragma once

#include "BitOperations.h"
#include "FoolsEngine\Debug\Asserts.h"

#include <vector>
#include <memory_resource>

namespace fe
{
	template <typename T>
	class Xar
	{
	public:
		Xar(std::pmr::polymorphic_allocator<std::byte> alloc) :
			m_Size(0), m_Chunks(alloc)
		{
			m_Chunks.push_back(alloc.allocate_object<T>(2));
		}

		T& operator[](size_t i)
		{
			FE_CORE_ASSERT(i < m_Size, "Out of bounds.");
			
			unsigned long chunk;
			auto is_i_non_zero = MSB64(&chunk, i);
			chunk = 0 + ((bool)is_i_non_zero) * chunk;
			auto chunk_mask = (uint64_t)1 << chunk;
			auto in_chunk_i = i - chunk_mask;
			auto result_ptr = (T*)(m_Chunks[chunk]) + in_chunk_i;
			return *result_ptr;
		}

		void push_back(T&& t)
		{
			unsigned long chunk;
			auto is_i_non_zero = MSB64(&chunk, m_Size);
			chunk = 0 + ((bool)is_i_non_zero) * chunk;
			if (chunk == m_Chunks.size())
			{
				m_Chunks.push_back(m_Chunks.get_allocator().allocate_object<T>(1<<chunk));

			}
			auto chunk_mask = (uint64_t)1 << chunk;
			auto in_chunk_i = m_Size - chunk_mask;

			auto result_ptr = (T*)(m_Chunks[chunk]) + in_chunk_i;
			// *result_ptr;
		}


	private:
		std::pmr::vector<T*> m_Chunks;
		uint32_t m_Size;
	};
}