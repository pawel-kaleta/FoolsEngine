#pragma once

#include "FoolsEngine/Foundation/DataTypes.h"
#include "FoolsEngine/Foundation/Utils/TypeSetup.h"
#include "InternalID.h"

#include <glad/gl.h>

namespace fe::Render::GAPI::OpenGL
{
	struct Buffer
	{
		constexpr static ObjType Type = ObjType::Buffer;
		GLuint mGLID = 0;
		U32 mSize = 0;
		bool mCommited = false;
		bool mReleased = false;

		FE_TYPE_SETUP(Buffer)

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

	struct DownStream;
	struct UpStream;

	struct Stream
	{
		InternalID mID;
		GLuint mOpenGLBuffer = 0;
		U32 mCapacity = 0;
		Byte* mDMABegin = nullptr;

		FE_TYPE_SETUP(Stream)

		DownStream* ToDown() { FE_CORE_ASSERT(mID.mComps.Type == ObjType::DownStream); return (DownStream*)this; }
		UpStream* ToUp() { FE_CORE_ASSERT(mID.mComps.Type == ObjType::UpStream); return (UpStream*)this; }

		struct Fence
		{
			GLsync OpenGLFence;
			Byte* Location;
		};
	};

	struct Region
	{
		constexpr static ObjType Type = ObjType::Region;
		Stream* mStream;
		Byte* mData;
		Stream::Fence* mFence;
		UInt mSize;

		FE_TYPE_SETUP(Region)

		U32 GetOffset() const
		{
			return mData - mStream->mDMABegin;
		}
	};
}