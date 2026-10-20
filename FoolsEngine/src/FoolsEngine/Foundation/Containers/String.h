#pragma once

#include "FoolsEngine/Foundation/DataTypes.h"

#include <cstring>

namespace fe
{
	struct String;

	struct CString
	{
		char* Data;
		UInt CountWithNull;

		CString() = default;
		CString(const char* string, UInt lengthWithNull) { FromConstCharPtr(string, lengthWithNull); }
		CString& operator=(const CString& other) = default;
		CString& operator=(CString&& other) = default;
		CString(const CString& other) = default;
		CString(CString&& other) = default;

		void Init()
		{
			Data = nullptr;
			CountWithNull = 0;
		}

		void FromConstCharPtr(const char* string, UInt lengthWithNull)
		{
			Data = (char*)string;
			CountWithNull = lengthWithNull;
		}

		inline String GetString();
	};

	struct String
	{
		char8_t* Data;
		UInt Count;

		String() = default;
		String(const char* string, UInt lengthWithNull) { FromConstCharPtr(string, lengthWithNull); }
		String(const char* string)
		{
			Data = (char8_t*)string;
			Count = 0;
			auto current = string;

			while (*current != '\0')
			{
				++Count;
				++current;
			}
		}

		void Init()
		{
			Data = nullptr;
			Count = 0;
		}

		void FromConstCharPtr(const char* string, UInt lengthWithNull)
		{
			Data = (char8_t*)string;
			Count = lengthWithNull-1;
		}
	};

	inline String operator""_FE(const char* str, std::size_t size)
	{
		UInt as = 5;
		return String(str, size+1);
	}

	inline String CString::GetString()
	{
		String result;
		result.Data = (char8_t*)Data;
		result.Count = CountWithNull - 1;
		return result;
	}

	inline bool CompareStringsEqual(String a, String b)
	{
		if (a.Count != b.Count)
			return false;

		return ! std::memcmp(a.Data, b.Data, a.Count);
	}
}

#include <spdlog/spdlog.h>

template<>
struct fmt::formatter<fe::String> : fmt::formatter<std::string>
{
	auto format(fe::String my, format_context& ctx) const -> decltype(ctx.out())
	{
		return fmt::format_to(ctx.out(), "[my_type i={}]", std::basic_string_view<char>((char*)my.Data, my.Count));
	}
};