#include "DockSpace.h"

namespace fe
{
	void DockSpace::BeginDockSpace()
	{
		FE_PROFILER_FUNC();

		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_::ImGuiDockNodeFlags_None;

		// nested docking spaces of the same size bad -> no docking to window, only to dedicated dockspace
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking;
		if (m_FullscreenOpt)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowBorderSize, 0.0f);

			window_flags |=
				ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_::ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_::ImGuiWindowFlags_NoMove |

				ImGuiWindowFlags_::ImGuiWindowFlags_NoBringToFrontOnFocus |
				ImGuiWindowFlags_::ImGuiWindowFlags_NoNavFocus;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_::ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		static bool is_dockspace_open = true;

		ImGui::Begin("Dockspace", &is_dockspace_open, window_flags); // ImGui::End() in EndDockSpace()
		
		ImGui::PopStyleVar();

		if (m_FullscreenOpt)
			ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float min_window_size_X = style.WindowMinSize.x;
		style.WindowMinSize.x = 300.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_::ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = min_window_size_X;
	}

	void DockSpace::EndDockSpace()
	{
		ImGui::End();
	}
}