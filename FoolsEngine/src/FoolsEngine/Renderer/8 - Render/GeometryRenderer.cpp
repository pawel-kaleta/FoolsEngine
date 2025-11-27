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
		void* VPmatrixPtr = (void*)glm::value_ptr(Renderer::SceneData.VPMatrix);

		auto& registry = scene.GetCoreComponent().GameplayWorld->GetRegistry();

		auto view_of_CRenderMeshView_components = registry.view<CRenderMeshView, CTransformGlobal>();
		for (auto ID : view_of_CRenderMeshView_components) // intellisense is freaking out here, don't worry, be happy
		{
			auto [comp_CRenderMeshView, component_CTransform] = view_of_CRenderMeshView_components.get(ID);
			if (!comp_CRenderMeshView.Material.IsValid())
				continue;
			if (!comp_CRenderMeshView.Mesh.IsValid())
				continue;
			if (comp_CRenderMeshView.Material.GetLoadingPriority() == AssetLoadingPriority::None)
				continue;
			if (comp_CRenderMeshView.Mesh.GetLoadingPriority() == AssetLoadingPriority::None)
				continue;

			auto material_observer = comp_CRenderMeshView.Material.Observe();
			auto mesh_observer = comp_CRenderMeshView.Mesh.Observe();

			if (!material_observer.AllOf<ACLoaded>())
				continue;
			if (!mesh_observer.AllOf<ACLoaded>())
				continue;

			glm::mat4 modelTransform = component_CTransform.GetRef().GetMatrix() * comp_CRenderMeshView.Offset.GetMatrix();
			void* modelTransformPtr = (void*)glm::value_ptr(modelTransform);

			AssetObserver<ShadingModel> shading_model_observer(material_observer.GetCoreComponent().ShadingModelID);
			auto shaderID = shading_model_observer.GetCoreComponent().ShaderID;

			{
				auto shader_observer = AssetObserver<Shader>(shaderID);

				shader_observer.Bind(GDI);

				shader_observer.UploadUniform(GDI, Uniform("u_ViewProjection", ShaderData::Type::Mat4), VPmatrixPtr);
				shader_observer.UploadUniform(GDI, Uniform("u_ModelTransform", ShaderData::Type::Mat4), modelTransformPtr);

				void* main_light_dir = glm::value_ptr(Renderer::SceneData.MainLight->Direction);
				shader_observer.UploadUniform(GDI, Uniform("u_MainLightDir", ShaderData::Type::Float3), main_light_dir);
				void* main_light_color = glm::value_ptr(Renderer::SceneData.MainLight->Color);
				shader_observer.UploadUniform(GDI, Uniform("u_MainLightColor", ShaderData::Type::Float3), main_light_color);
				void* main_light_intensity = &Renderer::SceneData.MainLight->Intensity;
				shader_observer.UploadUniform(GDI, Uniform("u_MainLightIntensity", ShaderData::Type::Float), main_light_intensity);

				void* ambient_light = glm::value_ptr(Renderer::SceneData.AmbientLight);
				shader_observer.UploadUniform(GDI, Uniform("u_AmbientLight", ShaderData::Type::Float3), ambient_light);

				void* camera_position = glm::value_ptr(Renderer::SceneData.CameraTransform.Shift);
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
		auto& registry = scene.GetCoreComponent().GameplayWorld->GetRegistry();
		auto view_of_CModelView_components = registry.view<CModel, CTransformGlobal>();
		auto& asset_registry = AssetManager::GetRegistry();

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

				glm::mat4 modelTransform = component_CTransform.GetRef().GetMatrix() * comp_CModel.Offset.GetMatrix();
				void* modelTransformPtr = (void*)glm::value_ptr(modelTransform);

				auto& model_core = model_observer.GetCoreComponent();
				for (auto rendermeshID : model_core.RenderMeshIDs)
				{
					auto& rendermesh_core = asset_registry.get<ACRenderMeshCore>(rendermeshID);
					AssetObserver<Material> material_observer(rendermesh_core.MaterialID);

					if (material_observer.GetCoreComponent().ShadingModelID == cutout_sm_observer.GetID())
					{
						FE_PROFILER_SCOPE("Mesh");

						AssetObserver<Mesh> mesh_observer(rendermesh_core.MeshID);
						cutout_shader_observer.UploadUniform(GDI, Uniform("u_ModelTransform", ShaderData::Type::Mat4), modelTransformPtr);
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

				glm::mat4 modelTransform = component_CTransform.GetRef().GetMatrix() * comp_CModel.Offset.GetMatrix();
				void* modelTransformPtr = (void*)glm::value_ptr(modelTransform);

				auto& model_core = model_observer.GetCoreComponent();
				for (auto rendermeshID : model_core.RenderMeshIDs)
				{
					auto& rendermesh_core = asset_registry.get<ACRenderMeshCore>(rendermeshID);
					AssetObserver<Material> material_observer(rendermesh_core.MaterialID);

					if (material_observer.GetCoreComponent().ShadingModelID == opaque_sm_observer.GetID())
					{
						FE_PROFILER_SCOPE("Mesh");

						AssetObserver<Mesh> mesh_observer(rendermesh_core.MeshID);
						opaque_shader_observer.UploadUniform(GDI, Uniform("u_ModelTransform", ShaderData::Type::Mat4), modelTransformPtr);
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