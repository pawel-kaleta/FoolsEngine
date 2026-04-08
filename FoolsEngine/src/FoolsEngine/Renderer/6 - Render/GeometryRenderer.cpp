#include "FE_pch.h"
#include "GeometryRenderer.h"

#include "FoolsEngine/Scene/Scene.h"
#include "FoolsEngine/Scene/Components/MeshComponents.h"

#include "FoolsEngine/Assets/AssetAccessors.h"

#include "FoolsEngine/Renderer/1 - Description/Library.h"
#include "FoolsEngine/Renderer/3 - Command/ResourceState.h"
#include "FoolsEngine/Renderer/7 - Integration/Renderer.h"

#include <glm/gtc/type_ptr.hpp>

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
		auto GAPI = Renderer::GetActiveGAPIType();
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

			AssetObserver<ShadingModel> shading_model_observer(material_observer.GetCore().ShadingModelID);

			const auto& lib = Description::Library::Get();
			{
				FE_CORE_ASSERT(false, "not implemented");
				//auto& program = shading_model_observer.GetResource<GAPIType::OpenGL>().Program;
				//
				////shader_observer.Bind(GAPI);
				//FE_CORE_ASSERT(false, "Bind program and textures!");
				//
				//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_ViewProjection", VP_matrix_ptr);
				//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_ModelTransform", model_transform_ptr);
				//
				//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_MainLightDir",  main_light_dir);
				//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_MainLightColor", main_light_color);
				//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_MainLightIntensity",  main_light_intensity);
				//
				//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_AmbientLight", ambient_light);
				//
				//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_CameraPosition", camera_position);
				//
				//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_EntityID", &ID);
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

		auto GAPI = Renderer::GetActiveGAPIType();
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

			FE_CORE_ASSERT(false, "not implemented");
			//auto& program = cutout_sm_observer.GetResource<GAPIType::OpenGL>().Program;
			//
			////cutout_shader_observer.Bind(GAPI);
			//FE_CORE_ASSERT(false, "Bind program and textures!");
			//
			//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_ViewProjection",		VPmatrixPtr);
			//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_MainLightDir",			main_light_dir);
			//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_MainLightColor",		main_light_color);
			//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_MainLightIntensity",	main_light_intensity);
			//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_AmbientLight",			ambient_light);
			//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_AmbientLightIntensity",	ambient_light_intensity);
			//
			//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_CameraPosition",		camera_position);


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

				auto& model_core = model_observer.GetCore();
				for (auto rendermeshID : model_core.RenderMeshIDs)
				{
					AssetObserver<RenderMesh> rendermesh_observer(rendermeshID);
					auto& rendermesh_core = rendermesh_observer.GetCore();
					AssetObserver<Material> material_observer(rendermesh_core.MaterialID);

					if (material_observer.GetCore().ShadingModelID == cutout_sm_observer.GetID())
					{
						FE_PROFILER_SCOPE("Mesh");

						AssetObserver<Mesh> mesh_observer(rendermesh_core.MeshID);
						FE_CORE_ASSERT(false, "not implemented");
						//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_ModelTransform",	model_transform_ptr);
						//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_EntityID",			&ID);

						mesh_observer.Draw(material_observer);
					}
				}
			}
		}

		{
			FE_PROFILER_SCOPE("Opaque geometry");

			AssetObserver<ShadingModel> opaque_sm_observer(Renderer::BaseAssets.ShadingModels.Base3DOpaque.GetID());

			FE_CORE_ASSERT(false, "not implemented");
			//auto& program = opaque_sm_observer.GetResource<GAPIType::OpenGL>().Program;
			//
			////opaque_shader_observer.Bind(GAPI);
			//FE_CORE_ASSERT(false, "Bind program and textures!");
			//
			//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_ViewProjection"			, VPmatrixPtr);
			//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_MainLightDir"			, main_light_dir);
			//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_MainLightColor"			, main_light_color);
			//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_MainLightIntensity"		, main_light_intensity);
			//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_AmbientLight"			, ambient_light);
			//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_AmbientLightIntensity"	, ambient_light_intensity);
			//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_CameraPosition"			, camera_position);


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

				auto& model_core = model_observer.GetCore();
				for (auto rendermeshID : model_core.RenderMeshIDs)
				{
					AssetObserver<RenderMesh> rendermesh_observer(rendermeshID);
					auto& rendermesh_core = rendermesh_observer.GetCore();
					AssetObserver<Material> material_observer(rendermesh_core.MaterialID);

					if (material_observer.GetCore().ShadingModelID == opaque_sm_observer.GetID())
					{
						FE_PROFILER_SCOPE("Mesh");

						AssetObserver<Mesh> mesh_observer(rendermesh_core.MeshID);
						FE_CORE_ASSERT(false, "not implemented");
						//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_ModelTransform", model_transform_ptr);
						//Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, "u_EntityID", &ID);

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