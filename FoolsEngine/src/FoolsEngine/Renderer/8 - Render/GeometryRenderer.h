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

	private:
		static void RenderCRenderMeshView(const AssetObserver<Scene>& scene);
		static void RenderCRenderMesh(const AssetObserver<Scene>& scene);
		static void RenderCModel(const AssetObserver<Scene>& scene);
		static void RenderCModelView(const AssetObserver<Scene>& scene);
	};
}