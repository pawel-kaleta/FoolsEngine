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

		enum DetectedMaterialProperties
		{
			BaseColor = BIT_FLAG(0),
			Ambient   = BIT_FLAG(1),
			Emissive  = BIT_FLAG(2),
			Metalness = BIT_FLAG(3),
			Roughness = BIT_FLAG(4),
			AlphaCutoff = BIT_FLAG(5),
			Opacity = BIT_FLAG(6)
			//= BIT_FLAG(6),
			//= BIT_FLAG(7),
			//= BIT_FLAG(8),
			//= BIT_FLAG(9),
			//= BIT_FLAG(10),
			//= BIT_FLAG(11),
			//= BIT_FLAG(12),
			//= BIT_FLAG(13)
		};
		
		struct MaterialData
		{
			AlphaMode AlphaMode;
			
			//std::pmr::vector<aiString>* DetectedTextures;
			Xar<aiString>* DetectedTextures;
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

			struct
			{
				glm::vec3 BaseColor;
				glm::vec3 Ambient;
				glm::vec3 Emissive;
				float Metalness;
				float Roughness;
				float AlphaCutoff;
				float Opacity;
			} Uniforms;

			uint32_t DetectedProperties;
		};

		struct Data
		{
			uint32_t PreviewItemSelectedIndex;
			bool GLTFTexturePacking;
			std::pmr::vector<MaterialData>* MaterialsData;
			ImportVariant ImportVariant;
			const aiScene* Scene;
		};
	};
}