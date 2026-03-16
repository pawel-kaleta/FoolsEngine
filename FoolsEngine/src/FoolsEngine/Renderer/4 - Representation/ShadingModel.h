#pragma once

#include "FoolsEngine/Assets/AssetHandle.h"
#include "FoolsEngine/Assets/AssetInterface.h"

#include "FoolsEngine/Renderer/1 - Description/Buffer.h"
#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"
#include "FoolsEngine/Renderer/1 - Description/ShaderInterface.h"
#include "FoolsEngine/Renderer/2 - Resource/Program.h"

namespace YAML { class Emitter; }

namespace fe
{
	struct ACShadingModelCore final : public AssetComponent
	{
		struct {
			AssetID Vertex;
			AssetID Tessellation;
			AssetID Geometry;
			AssetID Fragment;
		} ShaderIDs;

		void* DefaultUniformsData;
		size_t UniformsDataSize;

		uint32_t ProgramSpecificationID;

		void Init();

		~ACShadingModelCore() { if (DefaultUniformsData) operator delete(DefaultUniformsData); }
	};

	struct ACShaderModelResource_OpenGL final : public AssetComponent
	{
		Resource::Program_OpenGL Program;
	};

	class ShadingModelObserver : public AssetInterface
	{
	public:
		const ACShadingModelCore& GetCore() const { return Get<ACShadingModelCore>(); }

		template <GAPIType::ValueType GAPI>
		const auto& GetResourceComponent()
		{
			if constexpr (GAPI == GAPIType::OpenGL) return Get<ACShaderModelResource_OpenGL>();
		}

		const Description::Buffer::Layout& GetUniforms();

		size_t GetCPUDataSize() const { return GetCore().UniformsDataSize; }

		//consider iterator
		const void* GetUniformDefaultValuePtr(const ACShadingModelCore& dataComponent, const Description::Buffer::Element& targetUniform) const { return GetUniformDefaultValuePtr_Internal(dataComponent, targetUniform); };
		const void* GetUniformDefaultValuePtr(const ACShadingModelCore& dataComponent, const std::string& name) const { return GetUniformDefaultValuePtr_Internal(dataComponent, name); };

		void SaveMetadata(YAML::Emitter& emitter);

	protected:
		ShadingModelObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}

		void* GetUniformDefaultValuePtr_Internal(const ACShadingModelCore& dataComponent, const Description::Buffer::Element& targetUniform) const;
		void* GetUniformDefaultValuePtr_Internal(const ACShadingModelCore& dataComponent, const std::string& name) const;
	};

	class ShadingModelUser : public ShadingModelObserver
	{
	public:
		ACShadingModelCore& GetCore() const { return Get<ACShadingModelCore>(); }

		template <GAPIType::ValueType GAPI>
		auto& GetResourceComponent() const
		{
			if constexpr (GAPI == GAPIType::OpenGL) return Get<ACShaderModelResource_OpenGL>();
		}

		template <GAPIType::ValueType GAPI>
		auto& CreateResourceComponent() const
		{
			if constexpr (GAPI == GAPIType::OpenGL) return Get<ACShaderModelResource_OpenGL>();
		}

		void* GetUniformDefaultValuePtr(const ACShadingModelCore& dataComponent, const Description::Buffer::Element& targetUniform) const { return GetUniformDefaultValuePtr_Internal(dataComponent, targetUniform); };
		void* GetUniformDefaultValuePtr(const ACShadingModelCore& dataComponent, const std::string& name) const { return GetUniformDefaultValuePtr_Internal(dataComponent, name); };

		void SetUniformDefaultValue(const ACShadingModelCore& dataComponent, const Description::Buffer::Element& uniform, void* dataPointer) const;
		void SetUniformDefaultValue(const ACShadingModelCore& dataComponent, const std::string& name, void* dataPointer) const;

		bool LoadBaseAssetMetadata(const char* filepath);
		bool LoadMetadata();

		bool SendDataToGPU(GAPIType GAPI) const;
		void UnloadFromCPU() const {};
		void Release() const;

	protected:
		ShadingModelUser(ECS_AssetHandle ECS_handle) : ShadingModelObserver(ECS_handle) {}
	};

	class ShadingModel : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::ShadingModel; }
		static constexpr const char* GetMetaFileExtension() { return ".fesm"; }
		static void SaveMetadata(YAML::Emitter& emitter, AssetID assetID) {}
		static bool LoadMetadata(AssetID assetID) { return false; }
		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACShadingModelCore>(assetID).Init(); }

		using User = ShadingModelUser;
		using Observer = ShadingModelObserver;
		using Core = ACShadingModelCore;
	};
}