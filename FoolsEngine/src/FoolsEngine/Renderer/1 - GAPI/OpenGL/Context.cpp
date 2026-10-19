#include "FoolsEngine/Renderer/1 - GAPI/Context.h"
#include "Registry.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace fe::Context
{
	void InitGPU(U32 scrachBufferSize);
}

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
		case GL_DEBUG_SEVERITY_HIGH:         Context::Log::Core.Fatal("OpenGL:"_FE); break;
		case GL_DEBUG_SEVERITY_MEDIUM:       Context::Log::Core.Error("OpenGL:"_FE); break;
		case GL_DEBUG_SEVERITY_LOW:          Context::Log::Core.Warn("OpenGL:"_FE); break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: Context::Log::Core.Debug("OpenGL:"_FE); break;
		}

		Context::Log::Core.Debug("    Source:  {0}"_FE, Sources.at(source));
		Context::Log::Core.Debug("    Type:    {0}"_FE, Types.at(type));
		Context::Log::Core.Debug("    ID:      {0}"_FE, id);

		std::string logMessage = "    Message: ";
		logMessage += message;
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         Context::Log::Core.Fatal("{}"_FE, logMessage); break;
		case GL_DEBUG_SEVERITY_MEDIUM:       Context::Log::Core.Error("{}"_FE, logMessage); break;
		case GL_DEBUG_SEVERITY_LOW:          Context::Log::Core.Warn("{}"_FE, logMessage);  break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: Context::Log::Core.Debug("{}"_FE, logMessage); break;
		}

		if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
			FE_ASSERTION_BREAK();
	}

	GLFWwindow* BaseWindow = nullptr;

	bool Create()
	{
		// window creation
		{
			FE_PROFILER_SCOPE("GLFW_Create_BaseWindowInvisible");

#ifdef FE_INTERNAL_BUILD
			glfwWindowHint(GLFW_CONTEXT_DEBUG, GLFW_TRUE);
#endif // FE_INTERNAL_BUILD

			glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

			BaseWindow = glfwCreateWindow(32, 32, "", NULL, NULL); // arbytrary unimportant resolution - invisible placeholder window

			if (!BaseWindow)
			{
				FE_CORE_ASSERT(false, "GLFW BaseWindowInvisible creation failed!");
				return false;
			}

			glfwMakeContextCurrent(BaseWindow);

			glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE); // restore default
		}

		{
			FE_PROFILER_SCOPE("gladLoadGLLoader");
			int status = gladLoadGL((GLADloadfunc)glfwGetProcAddress);
			FE_CORE_ASSERT(status, "Failed to initialize glad - modern OpenGL loader!");
		}

		{
			FE_PROFILER_SCOPE("OpenGL platform info");
			Context::Log::Core.Info("Rendering platform info:"_FE);
			Context::Log::Core.Info("    Vendor:   {0}"_FE, (const char*)glGetString(GL_VENDOR));
			Context::Log::Core.Info("    Renderer: {0}"_FE, (const char*)glGetString(GL_RENDERER));
			Context::Log::Core.Info("    Version:  {0}"_FE, (const char*)glGetString(GL_VERSION));

			FE_CORE_ASSERT(GL_MAJOR_VERSION > 4 || (GL_MAJOR_VERSION == 4 && GL_MINOR_VERSION >= 5), "Minimal required OpenGL version is 4.5!");
		}

#ifdef FE_INTERNAL_BUILD
		{
			FE_PROFILER_SCOPE("OpenGL debug setup");
			glDebugMessageCallback(OpenGLMessageCallback, nullptr);
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_TRUE);
		}
#endif // FE_INTERNAL_BUILD

		Context::InitGPU(1024 * 64);

		Context::Log::Core.Info("OpenGL Rendering Context created"_FE);

		OpenGL::CreateRegistries();
	}

	void SetViewportCmd(U32 x, U32 y, U32 width, U32 height)
	{
		glViewport(x, y, width, height);
	}
}