#pragma once

namespace fe
{
	class Scene;

	template <typename tAssetType>
	class AssetObserver;

	class GeometryRenderer
	{
	public:
		static void Init() {};
		static void Shutdown() {};

		static void RenderScene(const AssetObserver<Scene>& scene);
	};
}