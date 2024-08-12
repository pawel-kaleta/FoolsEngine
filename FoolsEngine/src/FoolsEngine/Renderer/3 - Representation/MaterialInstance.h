#pragma once

#include "Material.h"

#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"

namespace fe
{
	struct ACMaterial final : public AssetComponent
	{
		AssetHandle<Material> Material;
	};

	struct ACUniformsData final : public AssetComponent
	{
		void* UniformsData = nullptr;
		size_t UniformsDataSize = 0;

		~ACUniformsData()
		{
			FE_PROFILER_FUNC();
			operator delete(UniformsData);
		}
	};

	struct ACTextures final : public AssetComponent
	{
		std::vector<AssetHandle<Texture2D>> Textures;
	};

	class MaterialInstance : public Asset
	{
	public:
		virtual AssetType GetType() { return GetTypeStatic(); }
		static AssetType GetTypeStatic() { return AssetType::MaterialInstanceAsset; }

		void Init(const AssetHandle<Material> material);

		virtual void UnloadFromGPU() override final { FE_CORE_ASSERT(false, "Not implemented"); };
		virtual void UnloadFromCPU() override final { FE_CORE_ASSERT(false, "Not implemented"); };

		const void* GetUniformValuePtr(const Uniform& targetUniform) const { return GetUniformValuePtr_Internal(targetUniform); };
		      void* GetUniformValuePtr(const Uniform& targetUniform)       { return GetUniformValuePtr_Internal(targetUniform); };

		const void* GetUniformValuePtr(const std::string& name) const { return GetUniformValuePtr_Internal(name); };
		      void* GetUniformValuePtr(const std::string& name)       { return GetUniformValuePtr_Internal(name); };;

		void SetUniformValue(const Uniform& uniform, void* dataPointer);
		void SetUniformValue(const std::string& name, void* dataPointer);

		AssetHandle<Texture2D> GetTexture(const ShaderTextureSlot& textureSlot) const;
		AssetHandle<Texture2D> GetTexture(const std::string& textureSlotName) const;
		void SetTexture(const ShaderTextureSlot& textureSlot, AssetHandle<Texture2D> texture);
		void SetTexture(const std::string& textureSlotName, AssetHandle<Texture2D> texture);

		const std::vector<AssetHandle<Texture2D>>& GetTextures() const { return Get<ACTextures>().Textures; }
		      std::vector<AssetHandle<Texture2D>>& GetTextures()       { return Get<ACTextures>().Textures; }
		
		AssetHandle<Material> GetMaterial() const { return Get<ACMaterial>().Material; }

		static void MakeMaterialInstance(AssetUser<MaterialInstance>& miUser);

	protected:
		friend class AssetObserver<MaterialInstance>;
		friend class AssetUser<MaterialInstance>;
		MaterialInstance(ECS_AssetHandle ECS_handle) : Asset(ECS_handle) {}

	private:

		void* MaterialInstance::GetUniformValuePtr_Internal(const Uniform& targetUniform) const;
		void* MaterialInstance::GetUniformValuePtr_Internal(const std::string& name) const;
	};
}