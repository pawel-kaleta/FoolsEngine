#pragma once

#include <filesystem>
#include <FoolsEngine.h>

#include "TextureImport.h"
#include "GeometryImport.h"

namespace fe
{
    struct ImportData
    {
        //char AssetName[64] = "";
        std::filesystem::path Filepath;
        //const std::vector<AssetID>* ImportedAssets = nullptr;
        //AssetHandleBase* HandleToOverride = nullptr;
        //AssetType Type = AssetType::None;
        const LoadersRegistry::Item* LoaderItemPtr = nullptr;
        std::pmr::monotonic_buffer_resource Arena;
        bool Finished = false;

        union
        {
            TextureImport::Data TextureData;
            GeometryImport::Data GeometryData;
        };
    };
}