#include "FE_pch.h"
#include "Mesh.h"

#include "FoolsEngine/Application/Project.h"

#include "FoolsEngine/Assets/Loaders/GeometryLoader.h"
#include "FoolsEngine/Assets/Serialization/YAML.h"

namespace fe::Render::Representation
{
	// mesh should not draw itself
	void MeshObserver::Draw(const AssetObserver<Material>& materialObserver) const
	{
		FE_CORE_ASSERT(false, "not implemented");
		FE_CORE_ASSERT(false, "mesh should not draw itself");

		//if (!AllOf<ACGPUBuffer>())
		//{
		//	//FE_CORE_ASSERT(false, "Mesh not uploaded to GPU");
		//	return;
		//}

		auto& material_core = materialObserver.GetCore();
		AssetObserver<ShadingModel> shading_model_observer(material_core.ShadingModelID);
		auto& sm_core = shading_model_observer.GetCore();

		const auto& library = Description::Library::Get();
		const auto& program_spec = library.ProgramSpecs[sm_core.ProgramSpecificationID];
		const auto uniforms_layout_id = program_spec.MainUniformsLayoutID;

		const auto& uniforms_layout = library.BufferLayouts[uniforms_layout_id];

		//auto& program = shading_model_observer.GetResource<GAPIType::OpenGL>().Program;

		//Description::Buffer::UniformBufferIterator uniform_it(uniforms_layout.Elements, material_core.UniformsData);
		//while (!uniform_it.IsEnd())
		//{
		//	Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, uniform_it.m_Index, uniform_it.Get());
		//	uniform_it.Move();
		//}

		RenderTextureSlotID renderer_texture_slot = 0;
		
		for (size_t i = 0; i < program_spec.TextureSamplerIDs.Count; ++i)
		{
			auto textureID = material_core.TextureIDs[i];
			auto& texture_sampler_id = program_spec.TextureSamplerIDs[i];
			const auto& texture_sampler = library.TextureSamplers[texture_sampler_id];

			if (textureID != NullAssetID)
			{
				AssetUser<Texture2D> texture(textureID);
				//const auto& texture_resource = texture.GetResource<GAPIType::OpenGL>().Texture;
				//Command::PipelineState::BindTextureToRendererTextureSlot<GAPIType::OpenGL>(renderer_texture_slot, texture_resource);
			}
			else
			{
				//const auto& texture_resource = Renderer::BaseAssets.Textures.Default.Use().GetResource<GAPIType::OpenGL>().Texture;
				//Command::PipelineState::BindTextureToRendererTextureSlot<GAPIType::OpenGL>(renderer_texture_slot, texture_resource);
			}

			//Command::ResourceState::BindTextureSamplerToRendererTextureSlot<GAPIType::OpenGL>((Resource::ProgramBase&)program, texture_sampler.Name, renderer_texture_slot);

			renderer_texture_slot++;
		}

		//const auto& gpuBuffers = Get<ACGPUBuffer>();

		//Command::PipelineState::BindVertexArray<GAPIType::OpenGL>(gpuBuffers.VertexArray);
		//Command::Render::DrawIndexed<GAPIType::OpenGL>(gpuBuffers.VertexArray);
	}
}