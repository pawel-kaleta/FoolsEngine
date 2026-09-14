#pragma once

#include "RBuffer.h"

#include "FoolsEngine/Renderer/1 - Description/Library.h"
#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"
#include "FoolsEngine/Foundation/Memory/DataTypes.h"

#include <glad/glad.h>

namespace fe::Resource
{
	using namespace Description;

	struct RMeshBindings
	{
		struct AttributeArray
		{
			RMemReg VertexData;
			U32 LayoutID;
			Splice<U08> AttributeBindingLocations;
		};

		RMemReg IndexData;
		U32 IndexCount; // need this?? recalculate as (IndexData.Size / 4) maybe?

		Splice<AttributeArray> AttributeArrays;

		virtual void Create() = 0;
		virtual void Delete() = 0;

		virtual void BindVertexData(UInt attributeArrayIndex) = 0;
		virtual void BindIndexData(RMemReg memReg, UInt indexCount) = 0;
	};

	struct RMeshBindings_OpenGL final : public RMeshBindings
	{
		GLuint OpenGLID;

		virtual void Create() override;
		virtual void Delete() override;

		virtual void BindVertexData(UInt attributeArrayIndex) override;
		virtual void BindIndexData(RMemReg memReg, UInt indexCount) override;
	};

}