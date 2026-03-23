#pragma once

#include "FoolsEngine/Foundation/Memory/Xar.h"

#include "FoolsEngine/Renderer/1 - Description/Buffer.h"

#include <glad/glad.h>

#include <queue>

namespace fe::Resource
{
	struct DownStream_OpenGL;

	struct StreamRegion { };

	struct DownStream
	{
		//Description::Buffer::Usage Usage; // do we need this?

		virtual void Create(size_t capacity) = 0;
		virtual void Destroy() = 0;

		virtual void BeginRegion() = 0;
		virtual void PushData(void* data, size_t size) = 0;
		virtual StreamRegion* EndRegion() = 0;

		virtual StreamRegion* ReserveUncommitedRegion(size_t size) = 0;
		virtual void CommitRegion(StreamRegion* region) = 0;

		virtual void RetireRegion(StreamRegion* region) = 0;

		virtual void EndFrame() = 0;
	};

	struct StreamRegion_OpenGL final : StreamRegion
	{
		DownStream_OpenGL* Stream;
		UInt Offset;
		GLuint OpenGLBuffer;
		U32 Size;
	};

	struct DownStream_OpenGL final : DownStream
	{
		static inline constexpr UInt Alignment = 64;

		struct Fence
		{
			GLsync OpenGLFence;
			UInt Location;
		};
		
		MonotonicAllocator Alloc;

		XarrAlloc<Fence, MonotonicAllocator> FrontFences;
		XarrAlloc<Fence, MonotonicAllocator> BackFences;
		UInt NextFenceIndex;
		
		XarrAlloc<StreamRegion_OpenGL, MonotonicAllocator> Regions;
		XarrAlloc<Fence*, MonotonicAllocator> RegionFences;

		UInt CurrentOffset;
		Byte* CPUMemoryBegin;
		UInt Capacity;
		GLuint OpenGLBuffer;

		// When we outgrow a buffer (hit a fence), we cannot orphan/recreate it,
		// because there may stil be unsceduled draw calls involving it
		// instead we postpone destruction to the end of the frame
		// when we are guaranteed, that all draw call are allready issued
		DynArr<GLuint> PastBuffersToDestroy;

		virtual void Create(UInt capacity) override;
		virtual void Destroy() override;

		virtual void BeginRegion() override;
		virtual void PushData(void* data, UInt size) override;
		virtual StreamRegion* EndRegion() override;

		virtual StreamRegion* ReserveUncommitedRegion(UInt size) override;
		virtual void CommitRegion(StreamRegion* region) override;

		virtual void RetireRegion(StreamRegion* region) override;

		virtual void EndFrame() override;

	private:
		bool CheckFences(UInt pushSize);
		void MakeNewBuffer(UInt pushSize);
	};
}