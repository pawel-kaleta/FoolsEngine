#include "FE_pch.h"

//tmp backend renderer for ImGui
#include <GLFW\glfw3.h>



#include "FoolsEngine\Core\Application.h"
#include "FoolsEngine\ImGui\ImGuiLayer.h"


namespace fe {

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
		FE_PROFILER_FUNC();
	}


	void ImGuiLayer::OnAttach()
	{
		FE_PROFILER_FUNC();

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
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()
	{
		FE_PROFILER_FUNC();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnEvent(Ref<Events::Event> event)
	{
		ImGuiIO& io = ImGui::GetIO();
		event->Handled |= event->IsInCategory(Events::Mouse) & io.WantCaptureMouse;
		event->Handled |= event->IsInCategory(Events::Keyboard) & io.WantCaptureKeyboard;
	}


	void ImGuiLayer::OnImGuiRender()
	{
		
	}

	void ImGuiLayer::Begin()
	{
		FE_PROFILER_FUNC();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		FE_PROFILER_FUNC();

		ImGuiIO& io = ImGui::GetIO();
		Window& window = Application::Get().GetWindow();
		io.DisplaySize = ImVec2((float)window.GetWidth(), (float)window.GetHeight());

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			// disconnecting ImGui from applications window creates a new platform window
			// updating and rendering them will change glfw context
			// so it needs to be backed and restored
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		//glfwSwapBuffers(static_cast<GLFWwindow*>(window.GetNativeWindow()));
	}
	
}