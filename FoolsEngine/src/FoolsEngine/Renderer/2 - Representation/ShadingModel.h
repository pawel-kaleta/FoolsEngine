#pragma once

#include "FoolsEngine/Assets/AssetHandle.h"
#include "FoolsEngine/Assets/AssetInterface.h"

#include "FoolsEngine/Renderer/1 - Description/Buffer.h"
#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"
#include "FoolsEngine/Renderer/1 - Description/ShaderInterface.h"
#include "FoolsEngine/Renderer/2 - Resource/RProgram.h"

namespace YAML { class Emitter; }

namespace fe
{
	struct ACShadingModelCore final : public AssetComponent
	{
		union {
			struct {
				AssetID Vertex;
				AssetID Tessellation;
				AssetID Geometry;
				AssetID Fragment;
			} ByName;
			Array<AssetID, 4> AsArray;
		} ShaderIDs;

		Splice<Byte> DefaultUniformsData;

		UInt ProgramSpecificationID;

		void Init();
	};

	struct ACRShadingModel_OpenGL final : public AssetComponent
	{
		Resource::RProgram_OpenGL Program;
	};

	class ShadingModelObserver : public AssetInterface
	{
	public:
		const ACShadingModelCore& GetCore() const { return Get<ACShadingModelCore>(); }

		const Description::Buffer::Layout& GetUniforms();

		Description::Buffer::UniformBufferIterator GetUniformDefaultValuesIterator();

		Splice<Byte> GetUniformDefaultValue(const Description::Buffer::Element& targetUniform) const;
		Splice<Byte> GetUniformDefaultValue(String name) const;

		void SaveMetadata(YAML::Emitter& emitter);

	protected:
		ShadingModelObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}
	};

	class ShadingModelUser : public ShadingModelObserver
	{
	public:
		ACShadingModelCore& GetCore() const { return Get<ACShadingModelCore>(); }

		void SetUniformDefaultValue(const Description::Buffer::Element& uniform, Splice<Byte> data) const;
		void SetUniformDefaultValue(String name, Splice<Byte> data) const;

		bool LoadBaseAssetMetadata(const char* filepath);
		bool LoadMetadata();

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