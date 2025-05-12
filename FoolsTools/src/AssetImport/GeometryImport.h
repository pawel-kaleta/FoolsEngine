#pragma once

#include <filesystem>
#include <FoolsEngine.h>

struct aiScene;

namespace fe
{
	struct ImportData;

	namespace GeometryImport
	{
		void RenderWindow(ImportData* const importData);

		void InitImport(ImportData* const importData);

		enum ImportVariant
		{
			ImportVariant_Prefab,
			ImportVariant_Model,
			ImportVariant_Mesh
		};

		struct Data
		{
			const aiScene* Scene;
			ImportVariant ImportVariant;
			uint32_t MaterialPreviewItemSelectedIndex;
			bool ImportMaterials;
			bool GLTFTexturePacking;
		};
	};
}