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
		Raster::Specification mRaster;
		UInt mRootDataSize;

		FE_TYPE_SETUP(GraphicsPipeline)

		void Init()
		{
			mOpenGLID = 0;
			mRootDataSize = 0;
			mRaster = Raster::Specification().Init();
		}

		void CreateCmd(const Shader& vertexShader, const Shader& fragmentShader, const Raster::Specification& spec, UInt rootDataSize)
		{
			FE_PROFILER_FUNC();

			mOpenGLID = glCreateProgram();
			mRaster = spec;
			mRootDataSize = rootDataSize;
				
			bool	cull_face_backup =			glIsEnabled(GL_CULL_FACE);
			GLint	cull_face_mode_backup = 0;	glGetIntegerv(GL_CULL_FACE_MODE, &cull_face_mode_backup);


			switch (spec.mFaceCullTest.Value)
			{
			case Raster::FaceCullTest::None:
				FE_CORE_ASSERT(false, "Unset facecull test");
				glDisable(GL_CULL_FACE);
			break;
			case Raster::FaceCullTest::Never:
				glDisable(GL_CULL_FACE);
			break;
			default:
				GLenum face_cull_mode;
				if		(mRaster.mFaceCullTest == Raster::FaceCullTest::Back)	face_cull_mode = GL_BACK;
				else if	(mRaster.mFaceCullTest == Raster::FaceCullTest::Front)	face_cull_mode = GL_FRONT;
				else if	(mRaster.mFaceCullTest == Raster::FaceCullTest::Always)	face_cull_mode = GL_FRONT_AND_BACK;
				glEnable(GL_CULL_FACE);
				glCullFace(face_cull_mode);
			}


			GLint drawFboId_backup = 0; glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFboId_backup);
			GLint readFboId_backup = 0; glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readFboId_backup);

			Framebuffer tmp_framebuffer;
			tmp_framebuffer.Create(spec.mFramebufferSpec, 4, 4);
			glBindFramebuffer(GL_FRAMEBUFFER, tmp_framebuffer.mOpenGLID);


			{
				FE_PROFILER_SCOPE("OpenGL Shader linking");

				glAttachShader(mOpenGLID, vertexShader.mOpenGLID);
				glAttachShader(mOpenGLID, fragmentShader.mOpenGLID);
				glLinkProgram(mOpenGLID);
			}


			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFboId_backup);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, readFboId_backup);
			tmp_framebuffer.Destroy();


			if (cull_face_backup)	glEnable(GL_CULL_FACE);
			else					glDisable(GL_CULL_FACE);
			glCullFace(cull_face_mode_backup);


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
			if (mRaster.mFaceCullTest == Raster::FaceCullTest::Never)
			{
				glDisable(GL_CULL_FACE);
			}
			else
			{
				glEnable(GL_CULL_FACE);
				
				if		(mRaster.mFaceCullTest == Raster::FaceCullTest::Back)	glCullFace(GL_BACK);
				else if (mRaster.mFaceCullTest == Raster::FaceCullTest::Front)	glCullFace(GL_FRONT);
				else if (mRaster.mFaceCullTest == Raster::FaceCullTest::Always)	glCullFace(GL_FRONT_AND_BACK);
			}

			glUseProgram(mOpenGLID);
		}
		
		void DestroyCmd()
		{
			glDeleteProgram(mOpenGLID);
		}
	};
}