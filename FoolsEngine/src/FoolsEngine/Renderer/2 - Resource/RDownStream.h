#pragma once

#include "FoolsEngine/Foundation/Memory/XarAlloc.h"
#include "FoolsEngine/Foundation/Memory/DynArrAlloc.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MonotonicAlloc.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MallocAlloc.h"

#include "FoolsEngine/Renderer/1 - Description/Buffer.h"
#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"

#include <glad/glad.h>

#include <queue>

namespace fe::Resource
{
	struct RStreamRegion {};

	struct RDownStream
	{
		//Description::Buffer::Usage Usage; // do we need this?

		virtual void Create(UInt capacity) = 0;
		virtual void Destroy() = 0;

		virtual void BeginRegion() = 0;
		virtual void PushData(Splice<Byte> data) = 0;
		virtual RStreamRegion* EndRegion() = 0;

		virtual RStreamRegion* ReserveUncommitedRegion(Splice<Byte> & CPURegion) = 0;
		virtual void CommitRegion(RStreamRegion* region) = 0;

		virtual void RetireRegion(RStreamRegion* region) = 0;

		virtual void EndFrame() = 0;
	};

	struct RDownStream_OpenGL;

	struct RStreamRegion_OpenGL final : public RStreamRegion
	{
		RDownStream_OpenGL* Stream;
		UInt Offset;
		GLuint OpenGLBuffer;
		U32 Size;
	};

	struct RDownStream_OpenGL final : public RDownStream
	{
		static inline constexpr UInt Alignment = 64;

		struct Fence
		{
			GLsync OpenGLFence;
			UInt Location;
		};
		
		MonotonicAlloc Alloc;

		XarrAlloc<Fence, MonotonicAlloc, Allocator> FrontFences;
		XarrAlloc<Fence, MonotonicAlloc, Allocator> BackFences;
		UInt NextFenceIndex;
		
		XarrAlloc<RStreamRegion_OpenGL, MonotonicAlloc, Allocator> Regions;
		XarrAlloc<Fence*, MonotonicAlloc, Allocator> RegionFences;

		UInt CurrentOffset;
		Byte* CPUMemoryBegin;
		UInt Capacity;
		GLuint OpenGLBuffer;

		// When we outgrow a buffer (hit a fence), we cannot orphan/recreate it,
		// because there may stil be unsceduled draw calls involving it
		// instead we postpone destruction to the end of the frame
		// when we are guaranteed, that all draw call are allready issued
		DynArrAlloc<GLuint, Allocator> PastBuffersToDestroy;

		virtual void Create(UInt capacity = 64) override;
		virtual void Destroy() override;

		virtual void BeginRegion() override;
		virtual void PushData(Splice<Byte> data) override;
		virtual RStreamRegion* EndRegion() override;

		virtual RStreamRegion* ReserveUncommitedRegion(Splice<Byte>& CPURegion) override;
		virtual void CommitRegion(RStreamRegion* region) override;

		virtual void RetireRegion(RStreamRegion* region) override;

		virtual void EndFrame() override;

	private:
		bool CheckFences(UInt pushSize);
		void MakeNewBuffer(UInt pushSize);
	};
}