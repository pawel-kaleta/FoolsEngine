#pragma once

#include "FoolsEngine/Foundation/Utils/Xar.h"

#include "FoolsEngine/Renderer/1 - Description/Buffer.h"

#include <glad/glad.h>

#include <queue>

namespace fe::Resource
{
	struct DownStream_OpenGL;

	struct StreamRegion { };

	struct DownStream
	{
		Description::Buffer::Usage Usage;

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
		GLuint OpenGLBuffer;
		uint32_t Offset;
		uint32_t Size;
	};

	struct DownStream_OpenGL final : DownStream
	{
		struct Fence
		{
			GLsync OpenGLFence;
			size_t Location;
		};

		// change std::queue to a custom ringbuffer queue,
		// std::queue is on top of std::deque and std::deque is bad https://godbolt.org/z/E53Ks8638
		// on top of that, you can't even clear std::deque, you have to recreate it
		//Note: beware of pointer stability in regards to RegionFences
		//maybe merge Fence and Region?
		std::vector<Fence> FrontFences;
		std::vector<Fence> BackFences;
		size_t NextFenceIndex;
		
		std::vector<StreamRegion_OpenGL> Regions;
		std::vector<Fence*> RegionFences;

		//size_t CurrentRegionOffset;
		size_t CurrentOffset;
		void* CPUMemoryBegin;
		size_t Capacity;
		GLuint OpenGLBuffer;

		// When we outgrow a buffer (hit a fence), we cannot orphan/recreate it,
		// because there may stil be unsceduled draw calls involving it
		// instead we postpone destruction to the end of the frame
		// when we are guaranteed, that all draw call are allready issued
		std::vector<GLuint> PastBuffersToDestroy;

		virtual void Create(size_t capacity) override;
		virtual void Destroy() override;

		virtual void BeginRegion() override;
		virtual void PushData(void* data, size_t size) override;
		virtual StreamRegion* EndRegion() override;

		virtual StreamRegion* ReserveUncommitedRegion(size_t size) override;
		virtual void CommitRegion(StreamRegion* region) override;

		virtual void RetireRegion(StreamRegion* region) override;

		virtual void EndFrame() override;

	private:
		bool CheckFences(size_t pushSize);
		void MakeNewBuffer(size_t pushSize);
	};
}