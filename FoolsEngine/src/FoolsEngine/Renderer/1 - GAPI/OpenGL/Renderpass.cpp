#include "FoolsEngine/Foundation/DataTypes.h"
#include "FoolsEngine/Renderer/1 - GAPI/Renderpass.h"
#include "Registry.h"

#include <glad/gl.h>

namespace fe::Render::GAPI
{
	namespace OpenGL::Utils
	{
		inline GLenum PrimitiveTypeToGLmode(Raster::PrimitiveType primitive)
		{
			// TO DO: make this a static lookup table?

			switch (primitive.Value)
			{
			case Raster::PrimitiveType::None:
				FE_CORE_ASSERT(false, "Not specified PrimitiveType");
				return GL_TRIANGLES;
			case Raster::PrimitiveType::Point:		return GL_POINTS;
			case Raster::PrimitiveType::Line:		return GL_LINES;
			case Raster::PrimitiveType::Triangle:	return GL_TRIANGLES;
			default:
				FE_CORE_ASSERT(false, "Uknown PrimitiveType");
				return GL_TRIANGLES;
			}
		}

		inline GLenum DepthTestTypeToGLenum(Descriptors::DepthStencil::DepthTestType test)
		{
			switch (test)
			{
			case DepthStencil::DepthTestType::None:
				FE_CORE_ASSERT(false, "Not specified PrimitiveType");
				return GL_GEQUAL;
			case DepthStencil::DepthTestType::Never:		return GL_NEVER;
			case DepthStencil::DepthTestType::Always:		return GL_ALWAYS;
			case DepthStencil::DepthTestType::NotEqual:		return GL_NOTEQUAL;
			case DepthStencil::DepthTestType::Less:			return GL_LESS;
			case DepthStencil::DepthTestType::LessEqual:	return GL_LEQUAL;
			case DepthStencil::DepthTestType::Equal:		return GL_EQUAL;
			case DepthStencil::DepthTestType::GreaterEqual:	return GL_GEQUAL;
			case DepthStencil::DepthTestType::Greater:		return GL_GREATER;
			default:
				FE_CORE_ASSERT(false, "Uknown DepthTestType");
				return GL_GEQUAL;
			}
		}

		inline GLenum BlendFunctionToGLEnum(Descriptors::Blend::BlendFunction func)
		{
			switch (func)
			{
			case Blend::BlendFunction::None:
				FE_CORE_ASSERT(false, "Not specified BlendFunction");
				return GL_NONE;
			case Blend::BlendFunction::SourceAlpha:			return GL_SRC_ALPHA;
			case Blend::BlendFunction::OneMinusSourceAlpha:	return GL_ONE_MINUS_SRC_ALPHA;

			default:
				Context::Log::Core.Error("Unrecognized BlendFunction"_FE);
				return GL_NONE;
			}
		}
	}

	Descriptors::FramebufferSpec sFramebufferSpec;

	UInt sRootDataSize;
	GLuint sRootDataSource;
	GLenum sMode;
	Descriptors::DepthStencil::Specification sDepthStencilState;
	Descriptors::Blend::Specification sBlendState;


	void BeginRenderpass(GID framebuffer)
	{
		OpenGL::Framebuffer* fb_obj = OpenGL::FramebufferRegistry.GetObj(framebuffer);
		sFramebufferSpec = fb_obj->mSpec;

		sRootDataSize = 0;
		sMode = GL_TRIANGLES;
		sDepthStencilState.Init();
		sBlendState.Init();
	}

	void ClearColor(UInt attachmentIndex, float value);
	void ClearColor(UInt attachmentIndex, SInt value);
	void ClearColor(UInt attachmentIndex, UInt value);
	void ClearDepth(float value);
	void ClearStencil(SInt value);
	void ClearDepthStencil(float depthValue, SInt stencilValue);

	void SetPipelineCmd(GID pipeline);
	void SetDepthStencilState(const Descriptors::DepthStencil::Specification& depthStencil);
	void SetBlendState(const Descriptors::Blend::Specification& blend);

	void SetRootSource(GID source)
	{
		switch ((ObjType::ValueType)((OpenGL::InternalID)source).mComps.Type)
		{
		case ObjType::Buffer:		sRootDataSource = OpenGL::BufferRegistry.GetObj(source)->mGLID; break;
		case ObjType::DownStream:	sRootDataSource = OpenGL::DownStreamRegistry.GetObj(source)->mOpenGLBuffer; break;
		case ObjType::UpStream:		FE_CORE_ASSERT(false, "Not implemented yet"); return;
		case ObjType::None:
		default:
			FE_CORE_ASSERT(false, "Urecognised type of source buffer for binding");
			return;
		}
	}

	void SetIndexSource(GID source)
	{
		GLuint buffer_gl;

		switch ((ObjType::ValueType)((OpenGL::InternalID)source).mComps.Type)
		{
		case ObjType::Buffer:		buffer_gl = OpenGL::BufferRegistry.GetObj(source)->mGLID; break;
		case ObjType::DownStream:	buffer_gl = OpenGL::DownStreamRegistry.GetObj(source)->mOpenGLBuffer; break;
		case ObjType::None:
		default:
			FE_CORE_ASSERT(false, "Urecognised type of source buffer binding");
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_gl);
	}

	void SetDrawParamsSource(GID source)
	{
		GLuint buffer_gl;

		switch ((ObjType::ValueType)((OpenGL::InternalID)source).mComps.Type)
		{
		case ObjType::Buffer:		buffer_gl = OpenGL::BufferRegistry.GetObj(source)->mGLID; break;
		case ObjType::DownStream:	buffer_gl = OpenGL::DownStreamRegistry.GetObj(source)->mOpenGLBuffer; break;
		case ObjType::None:
		default:
			FE_CORE_ASSERT(false, "Urecognised type of source buffer binding");
		}

		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer_gl);
	}

	void SetUniformSource(UInt bindingIndex, GID source, U32 offset, U32 size)
	{
		GLuint buffer_gl;

		switch ((ObjType::ValueType)((OpenGL::InternalID)source).mComps.Type)
		{
		case ObjType::Buffer:		buffer_gl = OpenGL::BufferRegistry.GetObj(source)->mGLID; break;
		case ObjType::DownStream:	buffer_gl = OpenGL::DownStreamRegistry.GetObj(source)->mOpenGLBuffer; break;
		case ObjType::None:
		default:
			FE_CORE_ASSERT(false, "Urecognised type of source buffer binding");
		}

		glBindBufferRange(GL_UNIFORM_BUFFER, bindingIndex, buffer_gl, offset, size);
	}

	void SetBufferSource(UInt bindingIndex, GID source)
	{

	}


	void DrawCmd(UInt primitiveCount, UInt indicesOffset, U32 rootOffset)
	{
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, sUniformSources[0], rootOffset, sRootDataSize);
		glDrawElements(sMode, primitiveCount, GL_UNSIGNED_INT, (void*)indicesOffset);
	}

	void DrawIndirectCmd(UInt paramsOffset, U32 rootOffset)
	{
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, sUniformSources[0], rootOffset, sRootDataSize);
		glDrawElementsIndirect(sMode, GL_UNSIGNED_INT, (void*)paramsOffset);
	}

	void MultiDrawIndirectCmd(UInt paramsOffset, UInt drawCount, U32 rootOffset)
	{
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, sUniformSources[0], rootOffset, sRootDataSize);
		glMultiDrawElementsIndirect(sMode, GL_UNSIGNED_INT, (void*)paramsOffset, drawCount, sizeof(DrawParams));
	}

	void MultiDrawIndirectCountCmd(UInt paramsOffset, UInt maxDrawCount, U32 rootOffset)
	{
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, sUniformSources[0], rootOffset, sRootDataSize);
		glMultiDrawElementsIndirectCount(sMode, GL_UNSIGNED_INT, (void*)(paramsOffset + 4), paramsOffset, maxDrawCount, sizeof(DrawParams));
	}


	////////////////////////

	// buffer, downstream or upstream
	void SetBuffer(GID pipeline, GID target, UInt bindingIndex)
	{
		FE_PROFILER_FUNC();

		OpenGL::GraphicsPipeline* pipeline_obj = OpenGL::GraphicsPipelineRegistry.GetObj(pipeline);
		GLuint buffer_gl;

		switch ((ObjType::ValueType)((OpenGL::InternalID)target).mComps.Type)
		{
		case ObjType::Buffer:		buffer_gl = OpenGL::BufferRegistry.GetObj(target)->mGLID; break;
		case ObjType::DownStream:	buffer_gl = OpenGL::DownStreamRegistry.GetObj(target)->mOpenGLBuffer; break;
		case ObjType::UpStream:		FE_CORE_ASSERT(false, "Not implemented yet"); return;
		case ObjType::None:
		default:
			FE_CORE_ASSERT(false, "Urecognised type of target for SSBO binding");
			return;
		}

		pipeline_obj->SetBuffer(buffer_gl, bindingIndex);
	}

	// buffer or downstream
	void SetIndexSource(GID pipeline, GID source)
	{
		FE_PROFILER_FUNC();

		OpenGL::GraphicsPipeline* pipeline_obj = OpenGL::GraphicsPipelineRegistry.GetObj(pipeline);
		GLuint buffer_gl;

		switch ((ObjType::ValueType)((OpenGL::InternalID)source).mComps.Type)
		{
		case ObjType::Buffer:		buffer_gl = OpenGL::BufferRegistry.GetObj(source)->mGLID; break;
		case ObjType::DownStream:	buffer_gl = OpenGL::DownStreamRegistry.GetObj(source)->mOpenGLBuffer; break;
		case ObjType::None:
		default:
			FE_CORE_ASSERT(false, "Urecognised type of target for SSBO binding");
		}

		pipeline_obj->SetIndicesBuffer(buffer_gl);
	}

	// buffer or downstream
	void SetDrawParamsSource(GID pipeline, GID source)
	{
		FE_PROFILER_FUNC();

		OpenGL::GraphicsPipeline* pipeline_obj = OpenGL::GraphicsPipelineRegistry.GetObj(pipeline);
		GLuint buffer_gl;

		switch ((ObjType::ValueType)((OpenGL::InternalID)source).mComps.Type)
		{
		case ObjType::Buffer:		buffer_gl = OpenGL::BufferRegistry.GetObj(source)->mGLID; break;
		case ObjType::DownStream:	buffer_gl = OpenGL::DownStreamRegistry.GetObj(source)->mOpenGLBuffer; break;
		case ObjType::None:
		default:
			FE_CORE_ASSERT(false, "Urecognised type of target for SSBO binding");
		}

		pipeline_obj->SetDrawParamsBuffer(buffer_gl);
	}
}