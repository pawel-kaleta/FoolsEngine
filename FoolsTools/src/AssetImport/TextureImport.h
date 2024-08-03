#pragma once

#include <filesystem>
namespace fe::TextureData
{
	struct Specification;
}

namespace fe
{
	class TextureImport
	{
	public:
		static void RenderWindow();
		static void OpenWindow(std::filesystem::path filepath);

	private:
		static std::filesystem::path s_Filepath;
		static TextureData::Specification s_Specification;
	};
}