#pragma once

namespace fe
{
	struct Tags
	{
		enum TagList : uint64_t
		{
			Error = WIDE_BIT_FLAG(0),
			Player = WIDE_BIT_FLAG(1)
		};

		Tags() = default;
		Tags(uint64_t tags)
			: TagBitFlags(tags) {};

		operator       uint64_t& ()       { return TagBitFlags; }
		operator const uint64_t& () const { return TagBitFlags; }

		Tags operator+ (Tags other)    const { return Tags(this->TagBitFlags |  other.TagBitFlags); }
		Tags operator- (Tags other)    const { return Tags(this->TagBitFlags & ~other.TagBitFlags); }

		Tags operator+ (TagList other) const { return operator+((Tags)other); }
		Tags operator- (TagList other) const { return operator-((Tags)other); }

		void operator+=(Tags other)          { this->TagBitFlags |=  other.TagBitFlags; }
		void operator-=(Tags other)          { this->TagBitFlags &= ~other.TagBitFlags; }

		void operator+=(TagList other)       { operator+=((Tags)other); }
		void operator-=(TagList other)       { operator-=((Tags)other); }

		bool operator==(Tags other)    const { return TagBitFlags == other.TagBitFlags; }

	private:
		friend class EntityInspector;

		uint64_t TagBitFlags = 0;
	};
}