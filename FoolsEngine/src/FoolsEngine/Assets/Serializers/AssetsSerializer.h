#pragma once

#include <filesystem>

namespace fe
{
	namespace AssetSerializer
	{
		void SerializeRegistry(const std::filesystem::path& filepath);
		bool DeserializeRegistry(const std::filesystem::path& filepath);

		void LoadMetaData();
	};
}