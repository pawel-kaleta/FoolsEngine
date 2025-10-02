#include "FE_pch.h"
#include "GeometryRenderer.h"

#include "FoolsEngine\Scene\Scene.h"
#include "FoolsEngine\Assets\AssetAccessors.h"

#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

#include "FoolsEngine\Scene\Components\MeshComponents.h"

#include <glm\gtc\type_ptr.hpp>

namespace fe
{
	void GeometryRenderer::RenderScene(const AssetObserver<Scene>& scene)
	{
		auto& registry = scene.GetCoreComponent().GameplayWorld->GetRegistry();
		void* VPmatrixPtr = (void*)glm::value_ptr(Renderer::SceneData.VPMatrix);
		auto GDI = Renderer::GetActiveGDItype();

		auto viewViewMeshes = registry.view<CRenderMeshView, CTransformGlobal>();
		for (auto ID : viewViewMeshes) // intellisense is freaking out here, don't worry, be happy
		{
			auto [renderViewMesh_component, transform_component] = viewViewMeshes.get(ID);
			if (!renderViewMesh_component.Material.IsValid())
				continue;
			if (!renderViewMesh_component.Mesh.IsValid())
				continue;
			if (renderViewMesh_component.Material.GetLoadingPriority() == AssetLoadingPriority::None)
				continue;
			if (renderViewMesh_component.Mesh.GetLoadingPriority() == AssetLoadingPriority::None)
				continue;

			auto material_observer = renderViewMesh_component.Material.Observe();
			auto mesh_observer = renderViewMesh_component.Mesh.Observe();

			if (!material_observer.AllOf<ACLoadedFlag>())
				continue;
			if (!mesh_observer.AllOf<ACLoadedFlag>())
				continue;

			glm::mat4 modelTransform = transform_component.GetRef().GetMatrix() * renderViewMesh_component.Offset.GetMatrix();
			void* modelTransformPtr = (void*)glm::value_ptr(modelTransform);

			AssetObserver<ShadingModel> shading_model_observer(material_observer.GetCoreComponent().ShadingModelID);
			auto shaderID = shading_model_observer.GetCoreComponent().ShaderID;

			{
				auto shader_observer = AssetObserver<Shader>(shaderID);

				shader_observer.Bind(GDI);
				shader_observer.UploadUniform(GDI, Uniform("u_ViewProjection", ShaderData::Type::Mat4), VPmatrixPtr);
				shader_observer.UploadUniform(GDI, Uniform("u_ModelTransform", ShaderData::Type::Mat4), modelTransformPtr);
				shader_observer.UploadUniform(GDI, Uniform("u_EntityID", ShaderData::Type::UInt), &ID);
			}

			mesh_observer.Draw(material_observer);
		}
	}
}