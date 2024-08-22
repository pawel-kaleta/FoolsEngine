#pragma once

namespace std::filesystem
{
	class path;
}

namespace fe
{
	class AssetHandleBase;

	class MeshImport
	{
	public:
		static void RenderWindow();

		static void InitImport(const std::filesystem::path& filepath, AssetHandleBase* optionalBaseHandle);
		static void ImportAs(const std::filesystem::path& filepath);

	private:
		static std::filesystem::path s_Filepath;
	};
}