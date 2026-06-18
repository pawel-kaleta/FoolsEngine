#include "FE_pch.h"
#include "FoolsEngine/Renderer/4 - Render Context/ResourceManager.h"

#include "FoolsEngine/Renderer/2 - Resource/RDownStream.h"
#include "FoolsEngine/Renderer/2 - Resource/RFramebuffer.h"
#include "FoolsEngine/Assets/AssetManager.h"

namespace fe
{
	bool ResourceManager_OpenGL::MakeStaticBuffer(RStaticBuffer_OpenGL& buffer, Splice<Byte> data)
	{
		FE_PROFILER_FUNC();

		buffer.Size = data.Count;
		buffer.Create();
		if (data.Elements)
			buffer.Update(0, data);

		return true;
	}
	bool ResourceManager_OpenGL::MakeDownStream(RDownStream_OpenGL& downStream, UInt initCapacity)
	{
		FE_PROFILER_FUNC();

		ContextScope scope(this);
		downStream.Create(initCapacity);

		return true;
	}
	bool ResourceManager_OpenGL::MakeFramebuffer(RFramebuffer_OpenGL& framebuffer, UInt specyficationID)
	{
		FE_PROFILER_FUNC();

		ContextScope scope(this);
		framebuffer.SpecificationID = specyficationID;
		framebuffer.Create();

		return true;
	}
	bool ResourceManager_OpenGL::MakeMeshBindings(RStaticBuffer_OpenGL& buffer, UInt offset, RMeshBindings_OpenGL& bindings, Splice<U32> indexData, Splice<Vert> vertexData)
	{
		FE_PROFILER_FUNC();

		if (!indexData.Elements || !vertexData.Elements)
			return false;

		Splice<Byte> index_mem_reg;
		index_mem_reg.Elements = (Byte*)indexData.Elements;
		index_mem_reg.Count = indexData.Count * sizeof(U32);
		UInt index_offset = offset;
		buffer.Update(offset, index_mem_reg);

		Splice<Byte> vertex_mem_reg;
		vertex_mem_reg.Elements = (Byte*)vertexData.Elements;
		vertex_mem_reg.Count = vertexData.Count * sizeof(Vert);
		UInt vertex_offset = offset + index_mem_reg.Count;
		buffer.Update(vertex_offset, vertex_mem_reg);
		
		bindings.LayoutID = Vert::GetLayoutID();
		bindings.Create();
		bindings.BindIndexData(buffer, index_offset, indexData.Count);
		bindings.BindVertexData(buffer, vertex_offset);

		return true;
	}
	
	bool ResourceManager_OpenGL::SendDataToGPU(RShader_OpenGL& shader, UInt specificationID, String source)
	{
		FE_PROFILER_FUNC();

		if (!source.IsValid() || source.IsEmpty())
			return false;

		shader.SpecificationID = specificationID;
		shader.Create(source);

		return true;
	}
	bool ResourceManager_OpenGL::SendDataToGPU(RProgram_OpenGL& program, UInt specificationID, Splice<RShader_OpenGL*> shaders)
	{
		FE_PROFILER_FUNC();

		ContextScope scope(this);
		program.SpecificationID = specificationID;

		program.ShaderOpenGLIDs = this->DefaultAlloc->Allocate(program.ShaderOpenGLIDs, shaders.Count);
		
		for (UInt i = 0; i < shaders.Count; i++)
		{
			FE_CORE_ASSERT(shaders[i], "Shouldnt be a nullptr");
			if (!shaders[i]) continue;
			program.ShaderOpenGLIDs[i] = shaders[i]->OpenGLID;
		}

		program.Create();

		return true;
	}
	bool ResourceManager_OpenGL::SendDataToGPU(RTexture_OpenGL& texture, const Description::Texture::Specification& specification, Splice<Byte> data)
	{
		FE_PROFILER_FUNC();

		if (!data.Elements)
			return false;

		texture.Create(specification, data);

		return true;
	}

//material
#if aaaa //move higher level
	bool ResourceManager_OpenGL::SendDataToGPU(Resource::StaticBuffer_OpenGL& buffer, UInt offset, )
	{
		FE_PROFILER_FUNC();

		if (core.ShadingModelID == NullAssetID)
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
#endif

	//render mesh
#if aaaa //move higher level
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
#endif

	void ResourceManager_OpenGL::ReleaseDataFromGPU(RShader_OpenGL& shader)
	{
		FE_PROFILER_FUNC();

		shader.Destroy();
	}
	void ResourceManager_OpenGL::ReleaseDataFromGPU(RProgram_OpenGL& program)
	{
		FE_PROFILER_FUNC();

		ContextScope scope(this);	
		program.Destroy();
		this->DefaultAlloc->Deallocate(program.ShaderOpenGLIDs);
	}
	void ResourceManager_OpenGL::ReleaseDataFromGPU(RTexture_OpenGL& texture)
	{
		FE_PROFILER_FUNC();

		texture.Destroy();
	}
	//material release
#ifdef aaaa //move higher level
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
#endif
	void ResourceManager_OpenGL::ReleaseDataFromGPU(RMeshBindings_OpenGL& bindings)
	{
		FE_PROFILER_FUNC();

		bindings.Delete();
	}
	void ResourceManager_OpenGL::ReleaseDataFromGPU(RStaticBuffer_OpenGL& buffer)
	{
		FE_PROFILER_FUNC();

		buffer.Delete();
	}
}