#include "FE_pch.h"

#include "FoolsEngine/Renderer/4 - Representation/Mesh.h"
#include "FoolsEngine/Renderer/5 - Render Context/ResourceManager.h"
#include "FoolsEngine/Assets/Loaders/TextureLoader.h"
#include "FoolsEngine/Assets/AssetManager.h"

namespace fe
{
	bool ResourceManagerOpenGL::SendDataToGPU(AssetUser<Shader      >& assetUser)
	{
		FE_PROFILER_FUNC();

		if (assetUser.AnyOf<ACShaderResource_OpenGL>())
		{
			FE_CORE_ASSERT(false, "Already on GPU");
			return false;
		}

		const auto& core = assetUser.GetCore();
		if (core.ShaderSource.empty())
		{
			return false;
		}

		auto& reg = AssetManager::Get().m_Registry;

		auto& shader_comp = reg.emplace<ACShaderResource_OpenGL>(assetUser.GetID());

		shader_comp.Shader.SpecificationID = core.SpecificationID;
		shader_comp.Shader.Create(core.ShaderSource.c_str());

		return true;
	}
	bool ResourceManagerOpenGL::SendDataToGPU(AssetUser<ShadingModel>& assetUser)
	{
		FE_PROFILER_FUNC();

		if (assetUser.AllOf<ACShaderModelResource_OpenGL>())
		{
			FE_CORE_ASSERT(false, "Already on GPU");
			return false;
		}

		ContextScope scope(this);

		auto& core = assetUser.GetCore();
		auto& reg = AssetManager::Get().m_Registry;

		auto& program_comp = reg.emplace<ACShaderModelResource_OpenGL>(assetUser.GetID());
		program_comp.Program.SpecificationID = core.ProgramSpecificationID;

		UInt shader_count
			= UInt(NullAssetID != core.ShaderIDs.AsArray[0])
			+ UInt(NullAssetID != core.ShaderIDs.AsArray[1])
			+ UInt(NullAssetID != core.ShaderIDs.AsArray[2])
			+ UInt(NullAssetID != core.ShaderIDs.AsArray[3]);

		program_comp.Program.Shaders = this->DefaultAlloc.Allocate<AssetID>(shader_count);
		
		for (UInt i = 0; i < shader_count; i++)
		{
			if (!core.ShaderIDs.AsArray[i]) continue;
			program_comp.Program.Shaders[i] = core.ShaderIDs.AsArray[i];
		}

		program_comp.Program.Create();

		return true;
	}
	bool ResourceManagerOpenGL::SendDataToGPU(AssetUser<Texture2D	>& assetUser)
	{
		FE_PROFILER_FUNC();

		if (assetUser.AllOf<ACTexture2DResource_OpenGL>())
		{
			FE_CORE_ASSERT(false, "Already on GPU");
			return false;
		}

		auto& core = assetUser.GetCore();
		if (!core.Data)
			return false;

		auto& reg = AssetManager::Get().m_Registry;

		auto& resource_comp = reg.emplace<ACTexture2DResource_OpenGL>(assetUser.GetID());
		resource_comp.Texture.Create(core.Specification, core.Data);

		return true;
	}
	bool ResourceManagerOpenGL::SendDataToGPU(AssetUser<Material	>& assetUser, Resource::StaticBuffer_OpenGL* buffer, uint32_t offset)
	{
		FE_PROFILER_FUNC();

		auto& core = assetUser.GetCore();

		if (core.ShadingModelID == NullAssetID)
			return false;

		UInt current_offset = offset;

		if (core.UniformBufferDataSize && core.UniformBufferData)
		{
			buffer->Update(current_offset, core.UniformBufferDataSize, core.UniformBufferData);
			current_offset += core.UniformBufferDataSize;
		}
		if (core.ShaderStorageDataSize && core.ShaderStorageData)
		{
			buffer->Update(current_offset, core.ShaderStorageDataSize, core.ShaderStorageData);
			current_offset += core.ShaderStorageDataSize;
		}

		for (const auto& texture_ID : core.TextureIDs)
		{
			if (texture_ID == NullAssetID)
				continue;

			AssetUser<Texture2D> texture_user(texture_ID);

			FE_CORE_ASSERT(texture_user.GetType() == AssetType::Texture2D, "Trying to load texture in material that is not a texture.");

			auto refs = texture_user.GetRefCounters();
			if (refs) //project asset
			{
				if (refs->LiveHandles[0].fetch_add(1) == 0)
				{
					if (!texture_user.IsLoaded())
					{
						TextureLoader::LoadTexture(texture_user);
						if (!SendDataToGPU(texture_user)) return false;
						texture_user.UnloadFromCPU();

						texture_user.FlagLoaded();
					}

					texture_user.FlagLoadedAsDependency();
				}
			}
			else //internal asset
			{
				FE_CORE_ASSERT(!texture_user.IsLoadedAsDependency(), "Internal Texture already marked LoadedAsDependency during loading");
				FE_CORE_ASSERT(!texture_user.IsLoaded(), "Internal Texture already marked Loaded during loading");

				TextureLoader::LoadTexture(texture_user);
				if (!SendDataToGPU(texture_user)) return false;
				texture_user.UnloadFromCPU();

				texture_user.FlagLoaded();
				texture_user.FlagLoadedAsDependency();
			}
		}

		return true;
	}
	bool ResourceManagerOpenGL::SendDataToGPU(AssetUser<Mesh		>& assetUser, Resource::StaticBuffer_OpenGL* buffer, uint32_t offset)
	{
		FE_PROFILER_FUNC();

		if (assetUser.AllOf<ACMeshBindings_OpenGL>())
		{
			FE_CORE_ASSERT(false, "Already on GPU");
			return false;
		}

		auto& core = assetUser.GetCore();

		if (!core.Data)
			return false;

		buffer->Update(offset, core.DataSize(), core.Data);

		auto& reg = AssetManager::Get().m_Registry;

		auto& mesh_bindings = reg.emplace<ACMeshBindings_OpenGL>(assetUser.GetID());
		
		mesh_bindings.MeshBindings.LayoutID = Description::Buffer::Vertex::GetLayoutID();
		mesh_bindings.MeshBindings.Create();
		mesh_bindings.MeshBindings.BindIndexBuffer(*buffer, offset, core.Specification.IndexCount);
		mesh_bindings.MeshBindings.BindVertexBuffer(*buffer, offset + ((intptr_t)core.GetVertexBufferPtr() - (intptr_t)core.Data));

		return true;
	}
	bool ResourceManagerOpenGL::SendDataToGPU(AssetUser<RenderMesh	>& assetUser, Resource::StaticBuffer_OpenGL* buffer, uint32_t offset)
	{
		FE_PROFILER_FUNC();

		auto& core = assetUser.GetCore();
		UInt current_offset = offset;

		// material loading
		{
			AssetUser<Material> material_user(core.MaterialID);

			auto refs = material_user.GetRefCounters();
			if (refs) // Project asset
			{
				if (refs->LiveHandles[0].fetch_add(1) == 0)
				{
					if (!material_user.IsLoaded())
					{
						if (!material_user.SendDataToGPU(GAPI))
							return false;

						material_user.FlagLoaded();
					}

					material_user.FlagLoadedAsDependency();
				}
			}
			else // internal asset
			{
				FE_CORE_ASSERT(!material_user.IsLoadedAsDependency(), "Internal Material already marked LoadedAsDependency during loading");
				FE_CORE_ASSERT(!material_user.IsLoaded(), "Internal Material already marked Loaded during loading");

				if (!this->SendDataToGPU(material_user, buffer, current_offset))
					return false;
				current_offset += material_user.GetGPUDataSize();

				material_user.FlagLoaded();
				material_user.FlagLoadedAsDependency();
			}

		}

		// mesh loading
		{
			AssetUser<Mesh> mesh_user(core.MeshID);

			auto refs = mesh_user.GetRefCounters();
			if (refs) // project asset
			{
				if (refs->LiveHandles[0].fetch_add(1) == 0)
				{
					if (!mesh_user.IsLoaded())
					{
						if (!mesh_user.SendDataToGPU(GAPI))
							return false;

						mesh_user.FlagLoaded();
					}
					mesh_user.FlagLoadedAsDependency();
				}
			}
			else // internal asset
			{
				FE_CORE_ASSERT(!mesh_user.IsLoadedAsDependency(), "Internal Mesh already marked LoadedAsDependency during loading");
				FE_CORE_ASSERT(!mesh_user.IsLoaded(), "Internal Mesh already marked Loaded during loading");

				if (!this->SendDataToGPU(mesh_user, buffer, current_offset))
					return false;
				current_offset += mesh_user.GetGPUDataSize();

				mesh_user.FlagLoaded();
				mesh_user.FlagLoadedAsDependency();
			}
		}

		return true;
	}
	bool ResourceManagerOpenGL::SendDataToGPU(AssetUser<Model		>& assetUser, Resource::StaticBuffer_OpenGL* buffer, uint32_t offset)
	{
		
	}

	void ResourceManagerOpenGL::ReleaseDataFromGPU(AssetUser<Shader      >& assetUser)
	{
		FE_PROFILER_FUNC();

		if (assetUser.AllOf<ACShaderResource_OpenGL>())
		{
			FE_CORE_ASSERT(false, "Not on GPU");
			return;
		}
		
		const auto& core = assetUser.GetCore();

		auto& reg = AssetManager::Get().m_Registry;

		auto shader_comp = reg.get<ACShaderResource_OpenGL>(assetUser.GetID());

		shader_comp.Shader.Destroy();
		reg.erase<ACShaderResource_OpenGL>(assetUser.GetID());
	}
	void ResourceManagerOpenGL::ReleaseDataFromGPU(AssetUser<ShadingModel>& assetUser)
	{
		FE_PROFILER_FUNC();

		if (assetUser.AllOf<ACShaderModelResource_OpenGL>())
		{
			FE_CORE_ASSERT(false, "Not on GPU");
			return;
		}

		auto& core = assetUser.GetCore();
		auto& reg = AssetManager::Get().m_Registry;

		auto program_comp = reg.get<ACShaderModelResource_OpenGL>(assetUser.GetID());

		ContextScope scope(this);
		program_comp.Program.Destroy();

		this->DefaultAlloc.Deallocate(program_comp.Program.Shaders);

		reg.erase<ACShaderModelResource_OpenGL>(assetUser.GetID());
	}
	void ResourceManagerOpenGL::ReleaseDataFromGPU(AssetUser<Texture2D   >& assetUser)
	{
		FE_PROFILER_FUNC();

		if (assetUser.AllOf<ACTexture2DResource_OpenGL>())
		{
			FE_CORE_ASSERT(false, "Already on GPU");
			return;
		}

		auto& reg = AssetManager::Get().m_Registry;

		auto& resource_comp = reg.emplace<ACTexture2DResource_OpenGL>(assetUser.GetID());
		resource_comp.Texture.Destroy();
		reg.erase<ACTexture2DResource_OpenGL>(assetUser.GetID());
	}
	void ResourceManagerOpenGL::ReleaseDataFromGPU(AssetUser<Material    >& assetUser)
	{
		FE_PROFILER_FUNC();

		if (assetUser.IsLoaded())
		{
			FE_CORE_ASSERT(false, "Already on GPU");
			return;
		}

		auto& core = assetUser.GetCore();

		for (const auto& texture_ID : core.TextureIDs)
		{
			if (texture_ID == NullAssetID)
				continue;

			AssetUser<Texture2D> texture_user(texture_ID);

			auto refs = texture_user.GetRefCounters();
			if (refs) // project asset
			{
				if (refs->LiveHandles[0].fetch_sub(1) == 1)
					texture_user.ReleaseDependencyLoad();
			}
			else // internal asset
			{
				texture_user.ReleaseDependencyLoad();
				ReleaseDataFromGPU(texture_user);
			}
		}
	}
	void ResourceManagerOpenGL::ReleaseDataFromGPU(AssetUser<Mesh        >& assetUser)
	{
		FE_PROFILER_FUNC();

		if (assetUser.AllOf<ACMeshBindings_OpenGL>())
		{
			FE_CORE_ASSERT(false, "Not on GPU");
			return;
		}

		auto& core = assetUser.GetCore();

		auto& reg = AssetManager::Get().m_Registry;

		auto& mesh_bindings = reg.get<ACMeshBindings_OpenGL>(assetUser.GetID());
		mesh_bindings.MeshBindings.Delete();
		reg.erase<ACMeshBindings_OpenGL>(assetUser.GetID());
	}
	void ResourceManagerOpenGL::ReleaseDataFromGPU(AssetUser<RenderMesh  >& assetUser)
	{

	}
	void ResourceManagerOpenGL::ReleaseDataFromGPU(AssetUser<Model       >& assetUser){}
}