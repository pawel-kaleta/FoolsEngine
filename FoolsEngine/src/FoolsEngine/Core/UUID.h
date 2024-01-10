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
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}