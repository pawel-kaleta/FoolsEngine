#pragma once

#include "FoolsEngine/Foundation/Memory/Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MallocAlloc.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MonotonicAlloc.h"
#include "FoolsEngine/Foundation/Memory/Xar.h"
#include "FoolsEngine/Foundation/Utils/Context.h"

#include "FoolsEngine/Renderer/1 - Description/Buffer.h"
#include "FoolsEngine/Renderer/2 - Resource/RBuffer.h"
#include "FoolsEngine/Renderer/2 - Resource/RDownStream.h"
#include "FoolsEngine/Renderer/2 - Resource/RFramebuffer.h"
#include "FoolsEngine/Renderer/2 - Resource/RShader.h"
#include "FoolsEngine/Renderer/2 - Resource/RProgram.h"
#include "FoolsEngine/Renderer/2 - Resource/RTexture.h"
#include "FoolsEngine/Renderer/2 - Resource/RMeshBindings.h"
#include "FoolsEngine/Renderer/3 - Command/ResourceState.h"

namespace fe
{
	struct ResourceManager { };

	using namespace Resource;
	struct ResourceManager_OpenGL final : public ResourceManager
	{
		Command::ResourceState_OpenGL Commands;

		TypedAlloc<Allocator>* DefaultAlloc;
		TypedAlloc<Allocator>* AuxilaryAlloc;
		TypedAlloc<MonotonicAlloc> PermanentAlloc;

		using Vert = Description::Buffer::Vertex;

		Xarr<RBuffer_OpenGL> GPBuffers;

		void Create()
		{
			DefaultAlloc = Context::Allocators::Default;
			AuxilaryAlloc = Context::Allocators::Default;
			PermanentAlloc.Init(Context::Allocators::Default);
		}

		RBuffer_OpenGL* MakeStaticBuffer(UInt capcity);
		RBuffer_OpenGL* MakeStaticBufferFill(Splice<Byte> data);

		bool MakeDownStream(RDownStream_OpenGL& downStream, UInt initCapacity = 64);
		bool MakeFramebuffer(RFramebuffer_OpenGL& framebuffer, UInt specyficationID);
		bool MakeMeshBindings(RBuffer_OpenGL& buffer, UInt offset, RMeshBindings_OpenGL& bindings, Splice<U32> indexData, Splice<Vert> vertexData);

		bool SendDataToGPU(RShader_OpenGL& shader, UInt specificationID, String source);
		bool SendDataToGPU(RProgram_OpenGL& program, UInt specificationID, Splice<RShader_OpenGL*> shaders);
		bool SendDataToGPU(RTexture_OpenGL& texture, const Description::Texture::Specification& specification, Splice<Byte> data);

		void ReleaseDataFromGPU(RShader_OpenGL& shader);
		void ReleaseDataFromGPU(RProgram_OpenGL& program);
		void ReleaseDataFromGPU(RTexture_OpenGL& texture);
		void ReleaseDataFromGPU(RMeshBindings_OpenGL& bindings);
		void ReleaseDataFromGPU(RBuffer_OpenGL& buffer);


		class ContextScope
		{
		public:
			ContextScope(ResourceManager_OpenGL* rm) :
				DefaultAlloc(Context::Allocators::Default),
				AuxilaryAlloc(Context::Allocators::Auxiliary)
			{
				Context::Allocators::Default = rm->DefaultAlloc;
				Context::Allocators::Auxiliary = rm->AuxilaryAlloc;
			}
			~ContextScope()
			{
				Context::Allocators::Default = DefaultAlloc;
				Context::Allocators::Auxiliary = AuxilaryAlloc;
			}
		private:
			TypedAlloc<Allocator>* DefaultAlloc;
			TypedAlloc<Allocator>* AuxilaryAlloc;
		};
	};
}