#pragma once

#include "FoolsEngine/Foundation/Memory/DataTypes.h"

#include <glad/gl.h>

namespace fe::GAPI::OpenGL
{
	struct Buffer
	{
		GLuint mGLID = 0;
		U32 mSize = 0;
		bool mCommited = false;
		bool mReleased = false;

		void Init()
		{
			mGLID = 0;
			mSize = 0;
			mCommited = false;
			mReleased = false;
		}

		Byte* Allocate(U32 size)
		{
			glCreateBuffers(1, &mGLID);

			GLbitfield create_flags = GL_MAP_WRITE_BIT;
			GLbitfield map_flags = GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT;

			glNamedBufferStorage(mGLID, size, nullptr, create_flags);
			Byte* CPUMemoryBegin = (Byte*)glMapNamedBufferRange(mGLID, 0, size, map_flags);
			mSize = size;
			return CPUMemoryBegin;
		}

		void AllocateCommit(U32 size)
		{
			glCreateBuffers(1, &mGLID);
			glNamedBufferStorage(mGLID, size, nullptr, 0);
			mSize = size;
			mCommited = true;
			return;
		}

		void Commit()
		{
			FE_CORE_ASSERT(!mCommited, "Allready commited!");
			glUnmapNamedBuffer(mGLID);
			mCommited = true;
		}

		void ReleaseCmd()
		{
			FE_CORE_ASSERT(!mReleased, "Allready commited!");
			glDeleteBuffers(1, &mGLID);
			mReleased = true;
			mGLID = 0;
		}
	};

	struct Stream
	{
		GLuint OpenGLBuffer = 0;
		U32 Capacity = 0;
		Byte* DMABegin = nullptr;

		struct Fence
		{
			GLsync OpenGLFence;
			Byte* Location;
		};
	};

	struct Region
	{
		UInt Size; // size first, as pool makes union of this with ptr of a freelist, its safer to not overlapp with ptrs in region
		Stream* Stream;
		Byte* Data;

		U32 GetOffset() const
		{
			return Data - Stream->DMABegin;
		}
	};
}