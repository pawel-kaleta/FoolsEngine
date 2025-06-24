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
			ImportVariant_Model,
			ImportVariant_RenderMesh,
			ImportVariant_Mesh
		};

		struct Data
		{
			const aiScene* Scene;
			ImportVariant ImportVariant;
			uint32_t MaterialPreviewItemSelectedIndex;
			//bool Merge;
			bool GLTFTexturePacking;
		};
	};
}