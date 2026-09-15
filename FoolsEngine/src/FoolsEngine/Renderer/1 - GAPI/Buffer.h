#pragma once

#include "FoolsEngine/Foundation/Utils/DeclareEnum.h"
#include "FoolsEngine/Foundation/Memory/DataTypes.h"

#include <glm/glm.hpp>

#include <glad/glad.h>

namespace fe::GAPI::Resource
{
	struct Buffer
	{
		GLuint mGLID = 0;
		U32 mSize = 0;
		bool mCommited = false;
		bool mReleased = false;

		void* Make(U32 size)
		{
			glCreateBuffers(1, &mGLID);

			GLbitfield create_flags = GL_MAP_WRITE_BIT;
			GLbitfield map_flags = GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT;

			glNamedBufferStorage(mGLID, size, nullptr, create_flags);
			void* CPUMemoryBegin = (Byte*)glMapNamedBufferRange(mGLID, 0, size, map_flags);
			mSize = size;
			return CPUMemoryBegin;
		};
		void Commit()
		{
			glUnmapNamedBuffer(mGLID);
			mCommited = true;
		}
		void Bind(U32 slot, U32 offset, U32 size)
		{
			glBindBufferRange(GL_SHADER_STORAGE_BUFFER, slot, mGLID, offset, size);
		}
		void Release()
		{
			glDeleteBuffers(1, &mGLID);
			mReleased = true;
			mGLID = 0;
		}
	};

}