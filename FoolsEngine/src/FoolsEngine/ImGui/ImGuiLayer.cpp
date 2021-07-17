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
		
		//io.
		

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
		io.DeltaTime = m_Time > 0.0 ? (time - m_Time) : (1.0f / 60.0f);
		
		ImGui_ImplOpenGL3_NewFrame();
		
		ImGui::NewFrame();
		
		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		

	}

	void ImGuiLayer::OnEvent(std::shared_ptr<Event> event)
	{
		
		EventDispacher dispacher(event);
		dispacher.Dispach<MouseButtonPressedEvent>	(std::bind(&ImGuiLayer::OnMouseButtonPressedEvent,	this, std::placeholders::_1));
		dispacher.Dispach<MouseButtonReleasedEvent>	(std::bind(&ImGuiLayer::OnMouseButtonReleasedEvent,	this, std::placeholders::_1));
		dispacher.Dispach<MouseMovedEvent>			(std::bind(&ImGuiLayer::OnMouseMovedEvent,			this, std::placeholders::_1));
		dispacher.Dispach<MouseScrolledEvent>		(std::bind(&ImGuiLayer::OnMouseScrolledEvent,		this, std::placeholders::_1));
		dispacher.Dispach<KeyPressedEvent>			(std::bind(&ImGuiLayer::OnKeyPressedEvent,			this, std::placeholders::_1));
		dispacher.Dispach<KeyReleasedEvent>			(std::bind(&ImGuiLayer::OnKeyReleasedEvent,			this, std::placeholders::_1));
		dispacher.Dispach<KeyTypedEvent>			(std::bind(&ImGuiLayer::OnKeyTypedEvent,			this, std::placeholders::_1));
		dispacher.Dispach<WindowResizeEvent>		(std::bind(&ImGuiLayer::OnWindowResizeEvent,		this, std::placeholders::_1));
		
	}

	bool ImGuiLayer::OnMouseButtonPressedEvent(std::shared_ptr<MouseButtonPressedEvent> e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e->GetMouseButtonCode()] = true;

		return false;
	}

	bool ImGuiLayer::OnMouseButtonReleasedEvent(std::shared_ptr < MouseButtonReleasedEvent> e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e->GetMouseButtonCode()] = false;

		return false;
	}

	bool ImGuiLayer::OnMouseMovedEvent(std::shared_ptr<MouseMovedEvent> e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(e->GetX(), e->GetY());

		return false;
	}

	bool ImGuiLayer::OnMouseScrolledEvent(std::shared_ptr<MouseScrolledEvent> e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheel	+= e->GetOffsetY();
		io.MouseWheelH	+= e->GetOffsetX();

		return false;
	}

	bool ImGuiLayer::OnKeyPressedEvent(std::shared_ptr<KeyPressedEvent> e)
	{
		return false;
	}

	bool ImGuiLayer::OnKeyReleasedEvent(std::shared_ptr<KeyReleasedEvent> e)
	{
		return false;
	}

	bool ImGuiLayer::OnKeyTypedEvent(std::shared_ptr<KeyTypedEvent> e)
	{
		return false;
	}

	bool ImGuiLayer::OnWindowResizeEvent(std::shared_ptr<WindowResizeEvent> e)
	{
		return false;
	}

	
	
}