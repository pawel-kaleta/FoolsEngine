#include "FE_pch.h"

//tmp backend renderer for ImGui
#include <GLFW\glfw3.h>
#include "FoolsEngine\Platform\OpenGL\imgui_impl_opengl3.h"

#include "FoolsEngine\Core\Application.h"
#include "FoolsEngine\ImGui\ImGuiLayer.h"
#include "FoolsEngine\Core\InputCodes.h"

namespace fe {

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{

	}

	ImGuiLayer::~ImGuiLayer()
	{

	}

	void ImGuiLayer::OnAttach()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsClassic();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		/*
		io.KeyMap[ImGuiKey_Tab] = InputCodes::Tab;
		io.KeyMap[ImGuiKey_LeftArrow] = InputCodes::Left;
		io.KeyMap[ImGuiKey_RightArrow] = InputCodes::Right;
		io.KeyMap[ImGuiKey_UpArrow] = InputCodes::Up;
		io.KeyMap[ImGuiKey_DownArrow] = InputCodes::Down;
		io.KeyMap[ImGuiKey_PageUp] = InputCodes::PageUp;
		io.KeyMap[ImGuiKey_PageDown] = InputCodes::PageDown;
		io.KeyMap[ImGuiKey_Home] = InputCodes::Home;
		io.KeyMap[ImGuiKey_End] = InputCodes::End;
		io.KeyMap[ImGuiKey_Insert] = InputCodes::Insert;
		io.KeyMap[ImGuiKey_Delete] = InputCodes::Delete;
		io.KeyMap[ImGuiKey_Backspace] = InputCodes::Backspace;
		io.KeyMap[ImGuiKey_Space] = InputCodes::Space;
		io.KeyMap[ImGuiKey_Enter] = InputCodes::Enter;
		io.KeyMap[ImGuiKey_Escape] = InputCodes::Escape;
		io.KeyMap[ImGuiKey_KeyPadEnter] = InputCodes::KPEnter;
		*/

		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()
	{

	}

	void ImGuiLayer::OnUpdate()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		float time = (float)glfwGetTime();
		io.DeltaTime = m_time > 0.0 ? (time - m_time): (1.0f / 60.0f);
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();
		
		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		

	}

	void ImGuiLayer::OnEvent(std::shared_ptr<Event> event)
	{

	}

}