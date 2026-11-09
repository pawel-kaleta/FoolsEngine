#pragma once

#include "FoolsEngine/Foundation/DataTypes.h"
#include "FoolsEngine/Foundation/Context/Pile.h"
#include "FoolsEngine/Foundation/Utils/TypeSetup.h"
#include "FoolsEngine/Renderer/1 - GAPI/Pipeline.h"
#include "FoolsEngine/Renderer/1 - GAPI/Data.h"
#include "Texture.h"
#include "Shader.h"
#include "Buffer.h"
#include "Framebuffer.h"

#include <glad/gl.h>

namespace fe::Render::GAPI::OpenGL
{
	struct GraphicsPipeline
	{
		constexpr static ObjType Type = ObjType::GraphicsPipeline;

		GLuint mOpenGLID;
		Descriptors::Raster::Spec mRaster;
		UInt mRootDataSize;

		FE_TYPE_SETUP(GraphicsPipeline)

		void Init()
		{
			mOpenGLID = 0;
			mRootDataSize = 0;
			mRaster.Init();
		}

		void CreateCmd(const Shader& vertexShader, const Shader& fragmentShader, const Descriptors::Raster::Spec& spec, UInt rootDataSize)
		{
			FE_PROFILER_FUNC();

			mOpenGLID = glCreateProgram();
			mRaster = spec;
			mRootDataSize = rootDataSize;


			if (spec.mFramebufferSpec.mDepthRange == Descriptors::Raster::DepthRange::ZeroToOne)
				glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
			if (spec.mFramebufferSpec.mDepthRange == Descriptors::Raster::DepthRange::MinusOneToOne)
				glClipControl(GL_LOWER_LEFT, GL_NEGATIVE_ONE_TO_ONE);

				
			bool	cull_face_backup =			glIsEnabled(GL_CULL_FACE);
			GLint	cull_face_mode_backup = 0;	glGetIntegerv(GL_CULL_FACE_MODE, &cull_face_mode_backup);


			switch (spec.mFaceCullTest.Value)
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
				if		(mRaster.mFaceCullTest == Descriptors::Raster::FaceCulling::Back)	face_cull_mode = GL_BACK;
				else if	(mRaster.mFaceCullTest == Descriptors::Raster::FaceCulling::Front)	face_cull_mode = GL_FRONT;
				else if	(mRaster.mFaceCullTest == Descriptors::Raster::FaceCulling::Always)	face_cull_mode = GL_FRONT_AND_BACK;
				glEnable(GL_CULL_FACE);
				glCullFace(face_cull_mode);
			}


			GLint drawFboId_backup = 0; glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFboId_backup);
			GLint readFboId_backup = 0; glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readFboId_backup);

			Framebuffer tmp_framebuffer;
			tmp_framebuffer.CreateCmd(spec.mFramebufferSpec, 4, 4);
			glBindFramebuffer(GL_FRAMEBUFFER, tmp_framebuffer.mOpenGLID);

			ArrayArena<Descriptors::Raster::ColorMask, 6> color_masks_backup;
			color_masks_backup.Count = spec.mColorWriteMasks.Count;

			for (UInt i=0; i<spec.mColorWriteMasks.Count; i++)
			{
				glGetBooleani_v(GL_COLOR_WRITEMASK, i, (GLboolean*) & (color_masks_backup[i]));
				auto& mask = spec.mColorWriteMasks[i];
				glColorMaski(i, mask.Red(), mask.Green(), mask.Blue(), mask.Alpha());
			}

			{
				FE_PROFILER_SCOPE("OpenGL Shader linking");

				glAttachShader(mOpenGLID, vertexShader.mOpenGLID);
				glAttachShader(mOpenGLID, fragmentShader.mOpenGLID);
				glLinkProgram(mOpenGLID);
			}

			for (UInt i = 0; i < spec.mColorWriteMasks.Count; i++)
			{
				auto& mask = color_masks_backup[i];
				glColorMaski(i, mask.Red(), mask.Green(), mask.Blue(), mask.Alpha());
			}

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFboId_backup);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, readFboId_backup);
			tmp_framebuffer.DestroyCmd();


			if (cull_face_backup)	glEnable(GL_CULL_FACE);
			else					glDisable(GL_CULL_FACE);
			glCullFace(cull_face_mode_backup);

			glClipControl(GL_LOWER_LEFT, GL_NEGATIVE_ONE_TO_ONE);


			GLint linking_success = 0;
			glGetProgramiv(mOpenGLID, GL_LINK_STATUS, (int*)&linking_success);
			if (linking_success == GL_FALSE)
			{
				GLint log_length = 0;
				glGetProgramiv(mOpenGLID, GL_INFO_LOG_LENGTH, &log_length);

				Pile p;
				auto info_log = p.Allocate<GLchar>(log_length);
				glGetProgramInfoLog(mOpenGLID, log_length, &log_length, info_log.Elements);

				glDeleteProgram(mOpenGLID);
				mOpenGLID = 0;

				Context::Log::Core.Error("{0}"_FE, info_log.Elements); // is this null terminated?
				FE_CORE_ASSERT(false, "OpenGL shader program linking failed!");
				return;
			}
			

			glDetachShader(mOpenGLID, vertexShader.mOpenGLID);
			glDetachShader(mOpenGLID, fragmentShader.mOpenGLID);


			// to do: test to see if we need a fake draw call (shader compilation may be defferred by opengl)
			//auto mode = Utils::PrimitiveTypeToGLmode(spec.mPrimitiveType);
			//fake draw call
			
			// to do: test if DepthStencilSpec and BlendSpec affect compilation
		}

		void ActivateCmd()
		{
			if (mRaster.mFaceCullTest == Descriptors::Raster::FaceCulling::Never)
			{
				glDisable(GL_CULL_FACE);
			}
			else
			{
				glEnable(GL_CULL_FACE);
				
				if		(mRaster.mFaceCullTest == Descriptors::Raster::FaceCulling::Back)	glCullFace(GL_BACK);
				else if (mRaster.mFaceCullTest == Descriptors::Raster::FaceCulling::Front)	glCullFace(GL_FRONT);
				else if (mRaster.mFaceCullTest == Descriptors::Raster::FaceCulling::Always)	glCullFace(GL_FRONT_AND_BACK);
			}

			glUseProgram(mOpenGLID);
		}
		
		void DestroyCmd()
		{
			glDeleteProgram(mOpenGLID);
		}
	};
}