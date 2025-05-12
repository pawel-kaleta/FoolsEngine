#pragma once

#include <filesystem>
#include <FoolsEngine.h>

#include "TextureImport.h"
#include "GeometryImport.h"

namespace fe
{
    struct ImportData
    {
        std::filesystem::path Filepath;
        AssetHandleBase* HandleToOverride = nullptr;
        AssetType Type = AssetType::None;
        uint32_t LoaderIndex = -1;
        bool Finished = false;

        union
        {
            TextureImport::Data TextureData;
            GeometryImport::Data GeometryData;
        };
    };
}