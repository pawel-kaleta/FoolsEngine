#include "FoolsEngine/Foundation/DataTypes.h"
#include "FoolsEngine/Renderer/1 - GAPI/Renderpass.h"
#include "Registry.h"

#include <glad/gl.h>

namespace fe::Render::GAPI
{
	namespace OpenGL::Utils
	{
		inline GLenum PrimitiveTypeToGLmode(Descriptors::Raster::PrimitiveType primitive)
		{
			// TO DO: make this a static lookup table?
			using namespace Descriptors::Raster;

			switch (primitive.Value)
			{
			case PrimitiveType::None:
				FE_CORE_ASSERT(false, "Not specified PrimitiveType");
				return GL_TRIANGLES;
			case PrimitiveType::Point:		return GL_POINTS;
			case PrimitiveType::Line:		return GL_LINES;
			case PrimitiveType::Triangle:	return GL_TRIANGLES;
			default:
				FE_CORE_ASSERT(false, "Uknown PrimitiveType");
				return GL_TRIANGLES;
			}
		}

		inline GLenum DepthTestTypeToGLenum(Descriptors::DepthStencil::DepthTestType test)
		{
			using namespace Descriptors::DepthStencil;

			switch (test)
			{
			case DepthTestType::None: FE_CORE_ASSERT(false, "AAA"); return GL_ALWAYS;
			case DepthTestType::Never:			return GL_NEVER;
			case DepthTestType::Always:			return GL_ALWAYS;
			case DepthTestType::NotEqual:		return GL_NOTEQUAL;
			case DepthTestType::Less:			return GL_LESS;
			case DepthTestType::LessEqual:		return GL_LEQUAL;
			case DepthTestType::Equal:			return GL_EQUAL;
			case DepthTestType::GreaterEqual:	return GL_GEQUAL;
			case DepthTestType::Greater:		return GL_GREATER;
			default: FE_CORE_ASSERT(false, "AAA"); return GL_ALWAYS;
			}
		}

		inline GLenum StencilTestTypeToGLenum(Descriptors::DepthStencil::StencilTestType test)
		{
			using namespace Descriptors::DepthStencil;

			switch (test)
			{
			case StencilTestType::None: FE_CORE_ASSERT(false, "AAA"); return GL_ALWAYS;
			case StencilTestType::Never:		return GL_NEVER;
			case StencilTestType::Always:		return GL_ALWAYS;
			case StencilTestType::NotEqual:		return GL_NOTEQUAL;
			case StencilTestType::Less:			return GL_LESS;
			case StencilTestType::LessEqual:	return GL_LEQUAL;
			case StencilTestType::Equal:		return GL_EQUAL;
			case StencilTestType::GreaterEqual:	return GL_GEQUAL;
			case StencilTestType::Greater:		return GL_GREATER;
			default: FE_CORE_ASSERT(false, "AAA"); return GL_ALWAYS;
			}
		}

		inline GLenum StencilOperationToGLenum(Descriptors::DepthStencil::StencilOperation op)
		{
			using namespace Descriptors::DepthStencil;

			switch (op.Value)
			{
			case StencilOperation::None: FE_CORE_ASSERT(false, "AAAA"); return GL_KEEP;
			case StencilOperation::Keep:			return GL_KEEP;
			case StencilOperation::Zero:			return GL_ZERO;
			case StencilOperation::Replace:			return GL_REPLACE;
			case StencilOperation::IncrementCap:	return GL_INCR;
			case StencilOperation::IncrementWrap:	return GL_INCR_WRAP;
			case StencilOperation::DecrementCap:	return GL_DECR;
			case StencilOperation::DecrementWrap:	return GL_DECR_WRAP;
			case StencilOperation::Invert:			return GL_INVERT;
			default: FE_CORE_ASSERT(false, "AAAA"); return GL_KEEP;
			}
		}

		inline GLenum BlendFunctionToGLEnum(Descriptors::Blend::BlendFunction func)
		{
			using namespace Descriptors::Blend;

			switch (func)
			{
			case BlendFunction::None:
				FE_CORE_ASSERT(false, "Not specified BlendFunction");
				return GL_NONE;
			case BlendFunction::SourceAlpha:			return GL_SRC_ALPHA;
			case BlendFunction::OneMinusSourceAlpha:	return GL_ONE_MINUS_SRC_ALPHA;

			default:
				Context::Log::Core.Error("Unrecognized BlendFunction"_FE);
				return GL_NONE;
			}
		}
	}

	Descriptors::Raster::FramebufferSpec sFramebufferSpec;

	GLuint sFramebuffer;
	GLint sDrawFboId_backup; 
	GLint sReadFboId_backup; 
	UInt sRootDataSize;
	GLuint sRootDataSource;
	GLenum sMode;
	

	void BeginRenderpassCmd(GID framebuffer)
	{
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &sDrawFboId_backup);
		glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &sReadFboId_backup);

		OpenGL::Framebuffer* fb_obj = OpenGL::FramebufferRegistry.GetObj(framebuffer);
		sFramebufferSpec = fb_obj->mSpec;

		sRootDataSize = 0;
		sMode = GL_TRIANGLES;
		sFramebuffer = fb_obj->mOpenGLID;

		glBindFramebuffer(GL_FRAMEBUFFER, fb_obj->mOpenGLID);
		glViewport(0, 0, fb_obj->mDimentions.x, fb_obj->mDimentions.y);
		
		if (fb_obj->mSpec.mDepthRange == Descriptors::Raster::DepthRange::ZeroToOne)
			glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
		else if (fb_obj->mSpec.mDepthRange == Descriptors::Raster::DepthRange::MinusOneToOne)
			glClipControl(GL_LOWER_LEFT, GL_NEGATIVE_ONE_TO_ONE);
	}

	void SetPipelineCmd(GID pipeline)
	{
		OpenGL::GraphicsPipeline* pipeline_obj = OpenGL::GraphicsPipelineRegistry.GetObj(pipeline);
		glUseProgram(pipeline_obj->mOpenGLID);
		sRootDataSize = pipeline_obj->mRootDataSize;
		sMode = OpenGL::Utils::PrimitiveTypeToGLmode(pipeline_obj->mRaster.mPrimitiveType);

		auto& raster = pipeline_obj->mRaster;

		switch (raster.mFaceCullTest.Value)
		{
		case Descriptors::Raster::FaceCulling::None:
			FE_CORE_ASSERT(false, "Unset facecull test");
			glDisable(GL_CULL_FACE);
			break;
		case Descriptors::Raster::FaceCulling::Never:
			glDisable(GL_CULL_FACE);
			break;
		default:
			GLenum face_cull_mode;
			if		(raster.mFaceCullTest == Descriptors::Raster::FaceCulling::Back)	face_cull_mode = GL_BACK;
			else if	(raster.mFaceCullTest == Descriptors::Raster::FaceCulling::Front)	face_cull_mode = GL_FRONT;
			else if	(raster.mFaceCullTest == Descriptors::Raster::FaceCulling::Always)	face_cull_mode = GL_FRONT_AND_BACK;
			glEnable(GL_CULL_FACE);
			glCullFace(face_cull_mode);
		}

		for (UInt i = 0; i < pipeline_obj->mRaster.mColorWriteMasks.Count; i++)
		{
			auto& mask = pipeline_obj->mRaster.mColorWriteMasks[i];
			glColorMaski(i, mask.Red(), mask.Green(), mask.Blue(), mask.Alpha());
		}
	}

	void SetDepthStencilStateCmd(const Descriptors::DepthStencil::Spec& spec)
	{
		using namespace Descriptors::DepthStencil;

		glDepthMask(spec.mDepthWrite);

		if (spec.mDepthTestType == DepthTestType::None)
		{
			glDisable(GL_DEPTH_TEST);
		}
		else
		{
			glEnable(GL_DEPTH_TEST);

			GLenum depth_test_type = OpenGL::Utils::DepthTestTypeToGLenum(spec.mDepthTestType);
			glDepthFunc(depth_test_type);
		}

		if (spec.mStencilTestType.Value == StencilTestType::None)
		{
			glDisable(GL_STENCIL_TEST);
		}
		else
		{
			FE_CORE_ASSERT(spec.mDepthTestType != DepthTestType::None, "Stencil Test requires Depth Test (can be DepthTestType::Always)");

			glEnable(GL_STENCIL_TEST);

			GLenum stencil_test_type = OpenGL::Utils::StencilTestTypeToGLenum(spec.mStencilTestType);
			glStencilFunc(stencil_test_type, spec.mStencilTestValue, spec.mStencilTestMask);

			GLenum s_fail			= OpenGL::Utils::StencilOperationToGLenum(spec.mStencilCases.Fail);
			GLenum s_pass__d_fail	= OpenGL::Utils::StencilOperationToGLenum(spec.mStencilCases.PassDepthFail);
			GLenum s_pass__d_pass	= OpenGL::Utils::StencilOperationToGLenum(spec.mStencilCases.PassDepthPass);
		
			glStencilOp(s_fail, s_pass__d_pass, s_pass__d_pass);
	
			glStencilMask(spec.mStencilWriteMask);
		}
	}

	void SetBlendStateCmd(const Descriptors::Blend::Spec& blend)
	{
		FE_CORE_ASSERT(false, "Not implemented!");
	}

	void ClearColorCmd(UInt attachmentIndex, Descriptors::Raster::ColorMask mask, const Array<F32, 4>& value)
	{
		GLboolean color_masks_backup[4];
		glGetBooleani_v(GL_COLOR_WRITEMASK, attachmentIndex, (GLboolean*)&(color_masks_backup));
		glColorMaski(attachmentIndex, mask.Red(), mask.Green(), mask.Blue(), mask.Alpha());
		glClearNamedFramebufferfv(sFramebuffer, GL_COLOR, attachmentIndex, value.begin());
		glColorMaski(attachmentIndex, color_masks_backup[0], color_masks_backup[1], color_masks_backup[2], color_masks_backup[3]);
	}
	void ClearColorCmd(UInt attachmentIndex, Descriptors::Raster::ColorMask mask, const Array<S32, 4>& value)
	{
		GLboolean color_masks_backup[4];
		glGetBooleani_v(GL_COLOR_WRITEMASK, attachmentIndex, (GLboolean*)&(color_masks_backup));
		glColorMaski(attachmentIndex, mask.Red(), mask.Green(), mask.Blue(), mask.Alpha());
		glClearNamedFramebufferiv(sFramebuffer, GL_COLOR, attachmentIndex, value.begin());
		glColorMaski(attachmentIndex, color_masks_backup[0], color_masks_backup[1], color_masks_backup[2], color_masks_backup[3]);
	}
	void ClearColorCmd(UInt attachmentIndex, Descriptors::Raster::ColorMask mask, const Array<U32, 4>& value)
	{
		GLboolean color_masks_backup[4];
		glGetBooleani_v(GL_COLOR_WRITEMASK, attachmentIndex, (GLboolean*)&(color_masks_backup));
		glColorMaski(attachmentIndex, mask.Red(), mask.Green(), mask.Blue(), mask.Alpha());
		glClearNamedFramebufferuiv(sFramebuffer, GL_COLOR, attachmentIndex, value.begin());
		glColorMaski(attachmentIndex, color_masks_backup[0], color_masks_backup[1], color_masks_backup[2], color_masks_backup[3]);
	}
	void ClearDepthCmd(float value)
	{
		GLboolean depth_write_mask_backup;
		glGetBooleanv(GL_DEPTH_WRITEMASK, &depth_write_mask_backup);
		glDepthMask(true);
		glClearDepthf(value);
		glClear(GL_DEPTH_BUFFER_BIT);
		glDepthMask(depth_write_mask_backup);
	}
	void ClearStencilCmd(U08 mask, U08 value)
	{
		GLint stencil_write_mask_backup;
		glGetIntegerv(GL_STENCIL_WRITEMASK, &stencil_write_mask_backup);
		glStencilMask(0b11111111);
		glClearStencil(value);
		glStencilMask(stencil_write_mask_backup);
	}

	void SetRootSourceCmd(GID source)
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

	void SetIndexSourceCmd(GID source)
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

	void SetDrawParamsSourceCmd(GID source)
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

	void SetUniformSourceCmd(UInt bindingIndex, GID source, U32 offset, U32 size)
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

	void SetBufferSourceCmd(UInt bindingIndex, GID source)
	{
		GLuint source_gl;
		GLsizeiptr source_size;

		switch ((ObjType::ValueType)((OpenGL::InternalID)source).mComps.Type)
		{
		case ObjType::Buffer:
		{
			auto buffer_obj = OpenGL::BufferRegistry.GetObj(source);
			source_gl = buffer_obj->mGLID;
			source_size = buffer_obj->mSize;
		}
		break;
		case ObjType::DownStream:
		{
			auto downstream_obj = OpenGL::DownStreamRegistry.GetObj(source);
			source_gl = downstream_obj->mOpenGLBuffer;
			source_size = downstream_obj->mCapacity;
		}
		break;
		case ObjType::None:
		default:
			FE_CORE_ASSERT(false, "Urecognised type of source buffer binding");
		}

		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, bindingIndex, source_gl, 0, source_size);
	}


	void DrawCmd(UInt primitiveCount, UInt indicesOffset, U32 rootOffset)
	{
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, sRootDataSource, rootOffset, sRootDataSize);
		glDrawElements(sMode, primitiveCount, GL_UNSIGNED_INT, (void*)indicesOffset);
	}

	void DrawIndirectCmd(UInt paramsOffset, U32 rootOffset)
	{
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, sRootDataSource, rootOffset, sRootDataSize);
		glDrawElementsIndirect(sMode, GL_UNSIGNED_INT, (void*)paramsOffset);
	}

	void MultiDrawIndirectCmd(UInt paramsOffset, UInt drawCount, U32 rootOffset)
	{
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, sRootDataSource, rootOffset, sRootDataSize);
		glMultiDrawElementsIndirect(sMode, GL_UNSIGNED_INT, (void*)paramsOffset, drawCount, sizeof(DrawParams));
	}

	void MultiDrawIndirectCountCmd(UInt paramsOffset, UInt maxDrawCount, U32 rootOffset)
	{
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, sRootDataSource, rootOffset, sRootDataSize);
		glMultiDrawElementsIndirectCount(sMode, GL_UNSIGNED_INT, (void*)(paramsOffset + 4), paramsOffset, maxDrawCount, sizeof(DrawParams));
	}

	void EndRenderpass()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, sDrawFboId_backup);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, sReadFboId_backup);

		glClipControl(GL_LOWER_LEFT, GL_NEGATIVE_ONE_TO_ONE);
	}
}