#include "FoolsEngine/Renderer/1 - GAPI/Context.h"
#include "Registry.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace fe::Render::GAPI
{
	// "cost" in order (may be driver internal cost of managing its own bookkeeping metadata and validation, not necesserily GPU state change)
	//Render target
	//Program
	//ROP (raster operations pipeline - blending/depth test/cull test/ etc.)
	// ------ state based recompilation boundary -------
	//texture sampler binding to binding point (sampler to render texture slot binding) - is there a need fot this ever?
	//texture bindings (different texture data format and filtering cause recompilation)
	//vertex layout
	// ------ possible state based recompilation boundary -------
	//uniform buffer binding
	//vertex / index binding
	//uniform update
	//draw call params

	void OpenGLMessageCallback(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam)
	{
		const static std::map<GLenum, const GLchar*> Sources =
		{
			{GL_DEBUG_SOURCE_API, "API"},
			{GL_DEBUG_SOURCE_WINDOW_SYSTEM, "WINDOW_SYSTEM"},
			{GL_DEBUG_SOURCE_SHADER_COMPILER, "SHADER_COMPILER"},
			{GL_DEBUG_SOURCE_THIRD_PARTY, "THIRD_PARTY"},
			{GL_DEBUG_SOURCE_APPLICATION, "APPLICATION"},
			{GL_DEBUG_SOURCE_OTHER, "OTHER"}
		};

		const static std::map<GLenum, const GLchar*> Types =
		{
			{GL_DEBUG_TYPE_ERROR, "ERROR"},
			{GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, "DEPRECATED_BEHAVIOR"},
			{GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, "UNDEFINED_BEHAVIOR"},
			{GL_DEBUG_TYPE_PORTABILITY, "PORTABILITY"},
			{GL_DEBUG_TYPE_PERFORMANCE, "PERFORMANCE"},
			{GL_DEBUG_TYPE_MARKER, "MARKER"},
			{GL_DEBUG_TYPE_PUSH_GROUP, "PUSH_GROUP"},
			{GL_DEBUG_TYPE_POP_GROUP, "POP_GROUP"},
			{GL_DEBUG_TYPE_OTHER, "OTHER"}
		};

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         Context->Log.FE.Fatal("OpenGL:"_FE); break;
		case GL_DEBUG_SEVERITY_MEDIUM:       Context->Log.FE.Error("OpenGL:"_FE); break;
		case GL_DEBUG_SEVERITY_LOW:          Context->Log.FE.Warn("OpenGL:"_FE); break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: Context->Log.FE.Debug("OpenGL:"_FE); break;
		}

		Context->Log.FE.Debug("    Source:  {0}"_FE, Sources.at(source));
		Context->Log.FE.Debug("    Type:    {0}"_FE, Types.at(type));
		Context->Log.FE.Debug("    ID:      {0}"_FE, id);

		std::string logMessage = "    Message: ";
		logMessage += message;
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         Context->Log.FE.Fatal("{}"_FE, logMessage); break;
		case GL_DEBUG_SEVERITY_MEDIUM:       Context->Log.FE.Error("{}"_FE, logMessage); break;
		case GL_DEBUG_SEVERITY_LOW:          Context->Log.FE.Warn("{}"_FE, logMessage);  break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: Context->Log.FE.Debug("{}"_FE, logMessage); break;
		}

		if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
			FE_ASSERTION_BREAK();
	}

	void Create()
	{
		FE_PROFILER_FUNC();

		{
			FE_PROFILER_SCOPE("gladLoadGLLoader");
			int status = gladLoadGL((GLADloadfunc)glfwGetProcAddress);
			FE_CORE_ASSERT(status, "Failed to initialize glad - modern OpenGL loader!");
			Context->Log.FE.Info("GAPI Loaded - OpenGL"_FE);
		}

		{
			FE_PROFILER_SCOPE("OpenGL debug");
			Context->Log.FE.Info("Rendering platform info:"_FE);
			Context->Log.FE.Info("    Vendor:   {0}"_FE, (const char*)glGetString(GL_VENDOR));
			Context->Log.FE.Info("    Renderer: {0}"_FE, (const char*)glGetString(GL_RENDERER));
			Context->Log.FE.Info("    Version:  {0}"_FE, (const char*)glGetString(GL_VERSION));

			FE_CORE_ASSERT(GL_MAJOR_VERSION > 4 || (GL_MAJOR_VERSION == 4 && GL_MINOR_VERSION >= 5), "Minimal required OpenGL version is 4.5!");

#ifdef FE_INTERNAL_BUILD

			glDebugMessageCallback(OpenGLMessageCallback, nullptr);
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_TRUE);
#endif // FE_INTERNAL_BUILD
		}

		{
			FE_PROFILER_SCOPE("Registries creation");
			OpenGL::CreateRegistries();
		}

		Context->Log.FE.Info("GAPI Registries created"_FE);
	}

	void SetViewportCmd(U32 x, U32 y, U32 width, U32 height)
	{
		glViewport(x, y, width, height);
	}
}