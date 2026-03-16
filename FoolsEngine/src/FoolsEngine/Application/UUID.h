#pragma once

#include <xhash>

namespace fe
{
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t value);
		UUID(const UUID&) = default;

		operator uint64_t() const { return m_Value; }
		void operator=(uint64_t other) { this->m_Value = other; }
		void operator=(const UUID& other) { this->m_Value = other.m_Value; }

		bool operator==(const UUID& other) const { return this->m_Value == other.m_Value; }
	private:
		uint64_t m_Value;
	};
}

namespace std
{
	template<>
	struct hash<fe::UUID>
	{
		std::size_t operator()(const fe::UUID& uuid) const
		{
			return hash<size_t>().operator()(uuid.operator size_t());
		}
	};
}