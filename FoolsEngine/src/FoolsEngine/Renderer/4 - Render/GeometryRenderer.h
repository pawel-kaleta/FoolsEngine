#pragma once

namespace fe
{
	class AssetObserver;

	class GeometryRenderer
	{
	public:
		static void Init() {};
		static void Shutdown() {};

		static void RenderScene(const AssetObserver& scene);

	private:
		static void RenderCRenderMeshView(const AssetObserver& scene);
		static void RenderCRenderMesh(const AssetObserver& scene);
		static void RenderCModel(const AssetObserver& scene);
		static void RenderCModelView(const AssetObserver& scene);
	};
}