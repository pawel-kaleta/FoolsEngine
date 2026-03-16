#include "FE_pch.h"
#include "Mesh.h"

#include "FoolsEngine/Application/Project.h"

#include "FoolsEngine/Assets/Loaders/GeometryLoader.h"
#include "FoolsEngine/Assets/Serialization/YAML.h"

#include "FoolsEngine/Renderer/3 - Command/ResourceState.h"
#include "FoolsEngine/Renderer/3 - Command/PipelineState.h"
#include "FoolsEngine/Renderer/3 - Command/Render.h"
#include "FoolsEngine/Renderer/7 - Integration/Renderer.h"

namespace fe
{
	void ACMeshCore::Init()
	{
		Specification.Init();
		if (Data) GeometryLoader::UnloadMesh(Data);
		Data = nullptr;
	}

	ACMeshCore::~ACMeshCore() { if (Data) GeometryLoader::UnloadMesh(Data); }

	bool MeshUser::SendDataToGPU(GAPIType GAPI) const
	{
		if (AllOf<ACGPUBuffer>())
		{
			FE_CORE_ASSERT(false, "Already on GPU");
			return false;
		}

		auto& core = Get<ACMeshCore>();

		if (!core.Data)
			return false;

		auto& buffer_comp = Emplace<ACGPUBuffer>();
		buffer_comp.Buffer.Usage = Description::Buffer::Usage::IndexVertex;
		buffer_comp.Buffer.Create();
		buffer_comp.Buffer.Upload(core.DataSize(), core.Data);

		return SendDataToGPUInternal(GAPI, &buffer_comp.Buffer, 0);
	}

	bool MeshUser::SendDataToGPUInternal(GAPIType GAPI, Resource::StaticBufferBase* buffer, uint32_t offset) const
	{
		auto& core = Get<ACMeshCore>();

		if (!core.Data)
			return false;

		buffer->Update(offset, core.DataSize(), core.Data);

		auto& vertex_array = Emplace<ACGPUVertexArray>();

		const auto& library = Description::Library::Get();
		auto program_spec_id = Renderer::BaseAssets.ShadingModels.Base3DOpaque.Observe().GetCore().ProgramSpecificationID;
		auto vertex_input_layout_id = library.ProgramSpecs[program_spec_id].VertexInputLayoutID;

		vertex_array.VertexArray.LayoutID = vertex_input_layout_id;
		vertex_array.VertexArray.Create();
		vertex_array.VertexArray.BindIndexBuffer(*buffer, offset, core.Specification.IndexCount);
		vertex_array.VertexArray.BindVertexBuffer(*buffer, offset + ((intptr_t)core.GetVertexBufferPtr() - (intptr_t)core.Data));

		return true;
	}

	void MeshUser::UnloadFromCPU() const
	{
		auto& data = Get<ACMeshCore>().Data;
		if (data)
		{
			GeometryLoader::UnloadMesh(data);
			data = nullptr;
		}
	}

	void MeshUser::Release() const
	{
		FE_CORE_ASSERT(AllOf<ACGPUVertexArray>(), "Mesh not loaded");

		Get<ACGPUVertexArray>().VertexArray.Delete();

		if (!AllOf<ACGPUBuffer>()) return;

		Get<ACGPUBuffer>().Buffer.Delete();
		Erase<ACGPUBuffer>();
	}

	// mesh should not draw itself
	void MeshObserver::Draw(const AssetObserver<Material>& materialObserver) const
	{
		FE_CORE_ASSERT(false, "mesh should not draw itself");

		if (!AllOf<ACGPUBuffer>())
		{
			//FE_CORE_ASSERT(false, "Mesh not uploaded to GPU");
			return;
		}

		auto& material_core = materialObserver.GetCore();
		AssetObserver<ShadingModel> shading_model_observer(material_core.ShadingModelID);
		auto& sm_core = shading_model_observer.GetCore();

		const auto& library = Description::Library::Get();
		const auto& program_spec = library.ProgramSpecs[sm_core.ProgramSpecificationID];
		const auto uniforms_layout_id = program_spec.MainUniformsLayoutID;

		const auto& uniforms_layout = library.BufferLayouts[uniforms_layout_id];

		auto& program = shading_model_observer.GetResourceComponent<GAPIType::OpenGL>().Program;

		Description::Buffer::UniformBufferIterator uniform_it(&uniforms_layout.Elements, material_core.UniformsData);
		while (!uniform_it.IsEnd())
		{
			Command::ResourceState::UploadUniform<GAPIType::OpenGL>((Resource::ProgramBase&)program, uniform_it.m_Index, uniform_it.Get());
			uniform_it.Move();
		}

		RenderTextureSlotID renderer_texture_slot = 0;
		
		for (size_t i = 0; i < program_spec.TextureSamplerIDs.size(); ++i)
		{
			auto textureID = material_core.TextureIDs[i];
			auto& texture_sampler_id = program_spec.TextureSamplerIDs[i];
			const auto& texture_sampler = library.TextureSamplers[texture_sampler_id];

			if (textureID != NullAssetID)
			{
				AssetUser<Texture2D> texture(textureID);
				const auto& texture_resource = texture.GetResourceComponent<GAPIType::OpenGL>().Texture;
				Command::PipelineState::BindTextureToRendererTextureSlot<GAPIType::OpenGL>(renderer_texture_slot, texture_resource);
			}
			else
			{
				const auto& texture_resource = Renderer::BaseAssets.Textures.Default.Use().GetResourceComponent<GAPIType::OpenGL>().Texture;
				Command::PipelineState::BindTextureToRendererTextureSlot<GAPIType::OpenGL>(renderer_texture_slot, texture_resource);
			}

			Command::ResourceState::BindTextureSamplerToRendererTextureSlot<GAPIType::OpenGL>((Resource::ProgramBase&)program, texture_sampler.Name, renderer_texture_slot);

			renderer_texture_slot++;
		}

		const auto& gpuBuffers = Get<ACGPUBuffer>();

		//Command::PipelineState::BindVertexArray<GAPIType::OpenGL>(gpuBuffers.VertexArray);
		//Command::Render::DrawIndexed<GAPIType::OpenGL>(gpuBuffers.VertexArray);
	}

	void Mesh::SaveMetadata(YAML::Emitter& emitter, AssetID assetID)
	{
		auto asset_observer = AssetObserver<Mesh>(assetID);
		auto& core = asset_observer.GetCore();

		emitter << YAML::BeginMap;
		emitter << YAML::Key << "UUID" << YAML::Value << asset_observer.GetUUID();
		if (!asset_observer.AllOf<ACMasterAsset>())
			emitter << YAML::Key << "Source Filepath" << YAML::Value << asset_observer.GetSourceFilepath()->Filepath.string();
		emitter << YAML::Key << "Vartex Count" << YAML::Value << core.Specification.VertexCount;
		emitter << YAML::Key << "Index Count" << YAML::Value << core.Specification.IndexCount;
		emitter << YAML::EndMap;
	}

	bool Mesh::LoadMetadata(AssetID assetID)
	{
		FE_PROFILER_FUNC();

		auto& reg = AssetManager::Get().m_Registry;

		const auto& filepath = reg.get<ACFilepath>(assetID).Filepath;
		auto full_filepath = Project::Get()->m_AssetsPath / filepath;

		YAML::Node node;

		{
			FE_PROFILER_SCOPE("YAML::LoadFile");
			node = YAML::LoadFile(full_filepath.string());
		}

		auto uuid_node = node["UUID"];
		if (uuid_node) // Base Assets don't have UUID in their file
		{
			if (reg.get<ACUUID>(assetID).UUID != node["UUID"].as<UUID>())
			{
				FE_CORE_ASSERT(false, "Not machting UUID in asset and its serialized node");
				return false;
			}
		}
		else
		{
			FE_LOG_CORE_WARN("Missing UUID in Mesh serialized node");
		}

		const auto& source_filepath_node = node["Source Filepath"];
		if (!source_filepath_node) return false;
		AssetManager::SetSourcePath(assetID, source_filepath_node.as<std::string>());

		const auto& vertex_count_node = node["Vartex Count"];
		const auto& index_count_node = node["Index Count"];

		if (!vertex_count_node ||
			!index_count_node)
			return false;

		auto& core = reg.get<ACMeshCore>(assetID);
		core.Specification.VertexCount = vertex_count_node.as<uint32_t>();
		core.Specification.IndexCount = index_count_node.as<uint32_t>();

		return true;
	}

	AssetID Mesh::LoadMetadataInternal(const YAML::Node& node, AssetID master, const std::filesystem::path& parentPath)
	{
		FE_PROFILER_FUNC();

		auto& reg = AssetManager::Get().m_Registry;

		const auto& uuid_node = node["UUID"];
		if (!uuid_node)
		{
			FE_LOG_CORE_WARN("Missing UUID in Mesh serialized node!");
			return NullAssetID;
		}

		auto uuid = uuid_node.as<UUID>();
		if (uuid == UUID(0))
		{
			FE_LOG_CORE_WARN("Missing Mesh definition!");
			return NullAssetID;
		}

		auto asset_id = AssetManager::GetOrCreateAssetWithUUID(uuid);
		if (reg.all_of<ACRefsCounters>(asset_id)) return asset_id; // is ProjectAsset ?

		reg.emplace<ACAssetType>(asset_id).Type = AssetType::Mesh;
		reg.emplace<ACMasterAsset>(asset_id).Master = master;
		auto& mesh_core = reg.emplace<Mesh::Core>(asset_id);
		mesh_core.Init();

		const auto& vertex_count_node = node["Vartex Count"];
		const auto& index_count_node = node["Index Count"];

		if (!vertex_count_node ||
			!index_count_node)
		{
			FE_LOG_CORE_WARN("Ill defined Mesh!");
			return NullAssetID;
		}

		mesh_core.Specification.VertexCount = vertex_count_node.as<uint32_t>();
		mesh_core.Specification.IndexCount = index_count_node.as<uint32_t>();

		return asset_id;
	}
}