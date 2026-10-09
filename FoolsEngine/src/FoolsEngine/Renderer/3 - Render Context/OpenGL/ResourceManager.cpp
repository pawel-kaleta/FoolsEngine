#include "FE_pch.h"

#include "FoolsEngine/Assets/AssetManager.h"
#include "FoolsEngine/Renderer/2 - Representation/Material.h"
#include "FoolsEngine/Renderer/2 - Representation/Texture.h"


namespace fe::Render::Representation
{

//material
	/*
	bool SendDataToGPU(UInt offset)
	{
		FE_PROFILER_FUNC();

		Material::Core core;

		if (core.mShadingModelID == NullAssetID)
			return false;

		UInt current_offset = offset;

		if (core.UniformBufferData.Count && core.UniformBufferData.Elements)
		{
			buffer.Update(current_offset, core.UniformBufferData);
			current_offset += core.UniformBufferData.Count;
		}
		if (core.ShaderStorageData.Count && core.ShaderStorageData.Elements)
		{
			buffer.Update(current_offset, core.ShaderStorageData);
			current_offset += core.ShaderStorageData.Count;
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

	//render mesh
	bool ResourceManager_OpenGL::SendDataToGPU(AssetUser<RenderMesh	>& assetUser,  Resource::StaticBuffer_OpenGL* buffer, uint32_t offset)
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


	//material release
	void ResourceManager_OpenGL::ReleaseDataFromGPU(AssetUser<Material    >& assetUser)
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
	*/
}