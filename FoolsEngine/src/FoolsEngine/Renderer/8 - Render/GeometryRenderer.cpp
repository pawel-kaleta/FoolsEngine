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
		RenderCRenderMeshView(scene);
		RenderCRenderMesh(scene);
		RenderCModel(scene);
		RenderCModelView(scene);
	}

	void GeometryRenderer::RenderCRenderMeshView(const AssetObserver<Scene>& scene)
	{
		auto GDI = Renderer::GetActiveGDItype();
		void* VP_matrix_ptr = (void*)glm::value_ptr(Renderer::SceneData.VPMatrix);

		auto& registry = scene.GetCoreComponent().GameplayWorld->m_Registry;
		
		void* main_light_dir = glm::value_ptr(Renderer::SceneData.MainLight->Direction);
		void* main_light_color = glm::value_ptr(Renderer::SceneData.MainLight->Color);
		void* main_light_intensity = &Renderer::SceneData.MainLight->Intensity;
		void* ambient_light = glm::value_ptr(Renderer::SceneData.AmbientLight);
		void* ambient_light_intensity = &Renderer::SceneData.AmbientLightIntensity;
		void* camera_position = glm::value_ptr(Renderer::SceneData.CameraTransform.Shift);

		auto view_of_CRenderMeshView_components = registry.view<CRenderMeshView, CTransformGlobal>();
		for (auto ID : view_of_CRenderMeshView_components) // intellisense is freaking out here, don't worry, be happy
		{
			auto [component_CRenderMeshView, component_CTransform] = view_of_CRenderMeshView_components.get(ID);
			if (!component_CRenderMeshView.Material.IsValid())
				continue;
			if (!component_CRenderMeshView.Mesh.IsValid())
				continue;
			if (component_CRenderMeshView.Material.GetLoadingPriority() == AssetLoadingPriority::None)
				continue;
			if (component_CRenderMeshView.Mesh.GetLoadingPriority() == AssetLoadingPriority::None)
				continue;

			auto material_observer = component_CRenderMeshView.Material.Observe();
			auto mesh_observer = component_CRenderMeshView.Mesh.Observe();

			if (!material_observer.AllOf<ACLoaded>())
				continue;
			if (!mesh_observer.AllOf<ACLoaded>())
				continue;

			glm::mat4 model_transform = component_CTransform.GetRef().GetMatrix() * component_CRenderMeshView.Offset.GetMatrix();
			void* model_transform_ptr = (void*)glm::value_ptr(model_transform);

			AssetObserver<ShadingModel> shading_model_observer(material_observer.GetCoreComponent().ShadingModelID);
			auto shaderID = shading_model_observer.GetCoreComponent().ShaderID;

			{
				auto shader_observer = AssetObserver<Shader>(shaderID);

				shader_observer.Bind(GDI);

				shader_observer.UploadUniform(GDI, Uniform("u_ViewProjection", ShaderData::Type::Mat4), VP_matrix_ptr);
				shader_observer.UploadUniform(GDI, Uniform("u_ModelTransform", ShaderData::Type::Mat4), model_transform_ptr);

				shader_observer.UploadUniform(GDI, Uniform("u_MainLightDir", ShaderData::Type::Float3), main_light_dir);
				shader_observer.UploadUniform(GDI, Uniform("u_MainLightColor", ShaderData::Type::Float3), main_light_color);
				shader_observer.UploadUniform(GDI, Uniform("u_MainLightIntensity", ShaderData::Type::Float), main_light_intensity);

				shader_observer.UploadUniform(GDI, Uniform("u_AmbientLight", ShaderData::Type::Float3), ambient_light);

				shader_observer.UploadUniform(GDI, Uniform("u_CameraPosition", ShaderData::Type::Float3), camera_position);

				shader_observer.UploadUniform(GDI, Uniform("u_EntityID", ShaderData::Type::UInt), &ID);
			}

			mesh_observer.Draw(material_observer);
		}
	}

	void GeometryRenderer::RenderCRenderMesh(const AssetObserver<Scene>& scene)
	{

	}

	void GeometryRenderer::RenderCModel(const AssetObserver<Scene>& scene)
	{
		FE_PROFILER_FUNC();

		auto GDI = Renderer::GetActiveGDItype();
		void* VPmatrixPtr = (void*)glm::value_ptr(Renderer::SceneData.VPMatrix);
		void* main_light_dir = glm::value_ptr(Renderer::SceneData.MainLight->Direction);
		void* main_light_color = glm::value_ptr(Renderer::SceneData.MainLight->Color);
		void* main_light_intensity = &Renderer::SceneData.MainLight->Intensity;
		void* ambient_light = glm::value_ptr(Renderer::SceneData.AmbientLight);
		void* ambient_light_intensity = &Renderer::SceneData.AmbientLightIntensity;
		void* camera_position = glm::value_ptr(Renderer::SceneData.CameraTransform.Shift);
		auto& registry = scene.GetCoreComponent().GameplayWorld->m_Registry;
		auto view_of_CModelView_components = registry.view<CModel, CTransformGlobal>();
		auto& asset_registry = AssetManager::Get().m_Registry;

		{
			FE_PROFILER_SCOPE("Cutout geometry");

			AssetObserver<ShadingModel> cutout_sm_observer(Renderer::BaseAssets.ShadingModels.Base3DCutout.GetID());
			auto cutout_shaderID = cutout_sm_observer.GetCoreComponent().ShaderID;
			auto cutout_shader_observer = AssetObserver<Shader>(cutout_shaderID);

			cutout_shader_observer.Bind(GDI);

			cutout_shader_observer.UploadUniform(GDI, Uniform("u_ViewProjection", ShaderData::Type::Mat4), VPmatrixPtr);
			cutout_shader_observer.UploadUniform(GDI, Uniform("u_MainLightDir", ShaderData::Type::Float3), main_light_dir);
			cutout_shader_observer.UploadUniform(GDI, Uniform("u_MainLightColor", ShaderData::Type::Float3), main_light_color);
			cutout_shader_observer.UploadUniform(GDI, Uniform("u_MainLightIntensity", ShaderData::Type::Float), main_light_intensity);
			cutout_shader_observer.UploadUniform(GDI, Uniform("u_AmbientLight", ShaderData::Type::Float3), ambient_light);
			cutout_shader_observer.UploadUniform(GDI, Uniform("u_AmbientLightIntensity", ShaderData::Type::Float), ambient_light_intensity);

			cutout_shader_observer.UploadUniform(GDI, Uniform("u_CameraPosition", ShaderData::Type::Float3), camera_position);


			for (auto ID : view_of_CModelView_components)
			{
				auto [comp_CModel, component_CTransform] = view_of_CModelView_components.get(ID);

				if (!comp_CModel.Model.IsValid())
					continue;

				auto model_observer = comp_CModel.Model.Observe();

				if (!model_observer.AllOf<ACLoaded>())
					continue;

				glm::mat4 model_transform = component_CTransform.GetRef().GetMatrix() * comp_CModel.Offset.GetMatrix();
				void* model_transform_ptr = (void*)glm::value_ptr(model_transform);

				auto& model_core = model_observer.GetCoreComponent();
				for (auto rendermeshID : model_core.RenderMeshIDs)
				{
					AssetObserver<RenderMesh> rendermesh_observer(rendermeshID);
					auto& rendermesh_core = rendermesh_observer.GetCoreComponent();
					AssetObserver<Material> material_observer(rendermesh_core.MaterialID);

					if (material_observer.GetCoreComponent().ShadingModelID == cutout_sm_observer.GetID())
					{
						FE_PROFILER_SCOPE("Mesh");

						AssetObserver<Mesh> mesh_observer(rendermesh_core.MeshID);
						cutout_shader_observer.UploadUniform(GDI, Uniform("u_ModelTransform", ShaderData::Type::Mat4), model_transform_ptr);
						cutout_shader_observer.UploadUniform(GDI, Uniform("u_EntityID", ShaderData::Type::UInt), &ID);

						mesh_observer.Draw(material_observer);
					}
				}
			}
		}

		{
			FE_PROFILER_SCOPE("Opaque geometry");

			AssetObserver<ShadingModel> opaque_sm_observer(Renderer::BaseAssets.ShadingModels.Base3DOpaque.GetID());
			auto opaque_shaderID = opaque_sm_observer.GetCoreComponent().ShaderID;
			auto opaque_shader_observer = AssetObserver<Shader>(opaque_shaderID);

			opaque_shader_observer.Bind(GDI);

			opaque_shader_observer.UploadUniform(GDI, Uniform("u_ViewProjection", ShaderData::Type::Mat4), VPmatrixPtr);
			opaque_shader_observer.UploadUniform(GDI, Uniform("u_MainLightDir", ShaderData::Type::Float3), main_light_dir);
			opaque_shader_observer.UploadUniform(GDI, Uniform("u_MainLightColor", ShaderData::Type::Float3), main_light_color);
			opaque_shader_observer.UploadUniform(GDI, Uniform("u_MainLightIntensity", ShaderData::Type::Float), main_light_intensity);
			opaque_shader_observer.UploadUniform(GDI, Uniform("u_AmbientLight", ShaderData::Type::Float3), ambient_light);
			opaque_shader_observer.UploadUniform(GDI, Uniform("u_AmbientLightIntensity", ShaderData::Type::Float), ambient_light_intensity);
			opaque_shader_observer.UploadUniform(GDI, Uniform("u_CameraPosition", ShaderData::Type::Float3), camera_position);


			for (auto ID : view_of_CModelView_components)
			{
				auto [comp_CModel, component_CTransform] = view_of_CModelView_components.get(ID);

				if (!comp_CModel.Model.IsValid())
					continue;

				auto model_observer = comp_CModel.Model.Observe();

				if (!model_observer.AllOf<ACLoaded>())
					continue;

				glm::mat4 model_transform = component_CTransform.GetRef().GetMatrix() * comp_CModel.Offset.GetMatrix();
				void* model_transform_ptr = (void*)glm::value_ptr(model_transform);

				auto& model_core = model_observer.GetCoreComponent();
				for (auto rendermeshID : model_core.RenderMeshIDs)
				{
					AssetObserver<RenderMesh> rendermesh_observer(rendermeshID);
					auto& rendermesh_core = rendermesh_observer.GetCoreComponent();
					AssetObserver<Material> material_observer(rendermesh_core.MaterialID);

					if (material_observer.GetCoreComponent().ShadingModelID == opaque_sm_observer.GetID())
					{
						FE_PROFILER_SCOPE("Mesh");

						AssetObserver<Mesh> mesh_observer(rendermesh_core.MeshID);
						opaque_shader_observer.UploadUniform(GDI, Uniform("u_ModelTransform", ShaderData::Type::Mat4), model_transform_ptr);
						opaque_shader_observer.UploadUniform(GDI, Uniform("u_EntityID", ShaderData::Type::UInt), &ID);

						mesh_observer.Draw(material_observer);
					}
				}
			}
		}
	}

	void GeometryRenderer::RenderCModelView(const AssetObserver<Scene>& scene)
	{

	}
}