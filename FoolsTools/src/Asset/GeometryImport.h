#pragma once

#include <filesystem>
#include <memory_resource>
#include <FoolsEngine.h>

struct aiScene;

namespace fe
{
	struct ImportData;

	namespace GeometryImport
	{
		void RenderWindow(ImportData* const importData);

		void InitImport(ImportData* const importData);

		FE_DECLARE_ENUM(ImportVariant, Model, RenderMesh, Mesh);


		struct Textures
		{
			Textures() = delete;

			uint32_t BaseColor;
			uint32_t Normal;
			uint32_t Emissive;
			union
			{
				uint32_t PackedOMR;
				struct
				{
					uint32_t Occlusion;
					uint32_t Metalness;
					uint32_t Roughness;
				} NonPackedOMR;
			};
		};
		
		struct MaterialData
		{
			AlphaMode AlphaMode;
			std::pmr::vector<aiString>* DetectedTextures;
			struct
			{
				uint32_t BaseColor;
				uint32_t Normal;
				uint32_t Emissive;
				union
				{
					uint32_t PackedOMR;
					struct
					{
						uint32_t Occlusion;
						uint32_t Metalness;
						uint32_t Roughness;
					} NonPackedOMR;
				};
			} RecognizedTextures;
		};

		struct Data
		{
			struct
			{
				bool GLTFTexturePacking;
				uint32_t PreviewItemSelectedIndex;
				std::pmr::vector<uint32_t>* SetTextures; // materials_count * 6
				std::pmr::vector<std::pmr::vector<aiString>>* RecognizedTextures;
			} Materials;
			std::pmr::vector<MaterialData>* MaterialsData;
			ImportVariant ImportVariant;
			const aiScene* Scene;
			
		};
	};
}