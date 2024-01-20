#pragma once

#include <string>
#include <filesystem>

namespace fe
{
	class FileDialogs
	{
	public:
		static std::filesystem::path OpenFile(const char* filter);
		static std::filesystem::path SaveFile(const char* defaultpath, const char* filter);
	};
}