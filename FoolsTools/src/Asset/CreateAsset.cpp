#include <FoolsEngine.h>

namespace fe
{
	namespace CreateAsset
	{
		bool modal_open = false;

		void Init()
		{

		}

		void OpenWindow()
		{
			modal_open = true;
		}

		void OnImGuiRender()
		{
			if (modal_open)
				ImGui::OpenPopup("Asset Creation");
			if (ImGui::BeginPopupModal("Asset Creation", NULL, 0))
			{

				if (ImGui::Button("Cancel"))
				{
					modal_open = false;
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}
	}
}