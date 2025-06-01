#include "FE_pch.h"
#include "FoolsEngine\ImGui\ImGuiLayer.h"

//tmp backend renderer for ImGui
#include <GLFW\glfw3.h>

#include "FoolsEngine\Events\Event.h"
#include "FoolsEngine\Core\Application.h"

#include <ImGuizmo.h>

namespace fe {

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
		FE_PROFILER_FUNC();
	}


	void ImGuiLayer::OnAttach()
	{
		FE_PROFILER_FUNC();

		m_Attached = true;
	}

	void ImGuiLayer::OnDetach()
	{
		FE_PROFILER_FUNC();

		m_Attached = false;
	}

	void ImGuiLayer::OnEvent(Ref<Events::Event> event)
	{
		if (m_BlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			event->Handled |= event->IsInCategory(Events::Mouse) & io.WantCaptureMouse;
			event->Handled |= event->IsInCategory(Events::Keyboard) & io.WantCaptureKeyboard;
			event->Owned = true;
		}
	}

	void ImGuiLayer::Startup()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer backends
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::GetWindow().GetNativeWindow());
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::Shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin()
	{
		FE_PROFILER_FUNC();

		if (!m_Attached)
		{
			FE_CORE_ASSERT(false, "ImGuiLayer not attached, but Begin() called");
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::End()
	{
		FE_PROFILER_FUNC();

		ImGuiIO& io = ImGui::GetIO();
		Window& window = Application::GetWindow();
		io.DisplaySize = ImVec2((float)window.GetWidth(), (float)window.GetHeight());

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			// disconnecting ImGui window from applications window creates a new platform window
			// updating and rendering them will change glfw context
			// so it needs to be backed up and restored
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::RenderUniform(const Uniform& uniform, void* uniformDataPtr, const UniformRenderSettings& options)
	{
		FE_PROFILER_FUNC();
		// TO DO: handle uniform.GetCount() > 1;

		auto name = uniform.GetName().c_str();
		ImGuiDataType ImGuiType = -1;

		switch (uniform.GetPrimitive().Value)
		{
		case ShaderData::Primitive::None:
			FE_CORE_ASSERT(false, "Unknown Shader Data Primitive of uniform!");
			return;
		
		case ShaderData::Primitive::Bool:
		{
			bool* dataPtr = (bool*)uniformDataPtr;
			for (unsigned int i = 1; i < uniform.GetCount(); i++)
			{
				ImGui::Checkbox("", dataPtr++); ImGui::SameLine();
			}
			ImGui::Checkbox(name, dataPtr);
			return;
		}

		case ShaderData::Primitive::Int:
			ImGuiType = ImGuiDataType_::ImGuiDataType_S32;
			break;
		case ShaderData::Primitive::UInt:
			ImGuiType = ImGuiDataType_::ImGuiDataType_U32;
			break;
		case ShaderData::Primitive::Float:
			ImGuiType = ImGuiDataType_::ImGuiDataType_Float;
			break;
		case ShaderData::Primitive::Double:
			ImGuiType = ImGuiDataType_::ImGuiDataType_Double;
			break;

		default:
			FE_CORE_ASSERT(false, "Unrecognised Shader Data Primitive of uniform!");
			return;
		}

		int count = (int)ShaderData::SizeOfType(uniform.GetType()) / ShaderData::SizeOfPrimitive(uniform.GetPrimitive());
		ImGui::DragScalarN(name, ImGuiType, uniformDataPtr, count, options.Speed, options.MinValue, options.MaxValue, options.Format, options.Flags);
	}
	
}