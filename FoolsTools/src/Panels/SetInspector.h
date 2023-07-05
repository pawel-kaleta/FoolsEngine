#pragma once

#include <FoolsEngine.h>

namespace fe
{
	class SetInspector
	{
	public:
		SetInspector() = default;
		SetInspector(const Ref<Scene>& scene)
			: m_Scene(scene) {}

		void SetScene(const Ref<Scene>& scene);

		void OpenSet(SetID setID);
		void OnImGuiRender();
	private:
		Ref<Scene> m_Scene;
		SetID m_OpenedSetID = NullSetID;

		void RenderCNameWidget(Set set);
		void RenderCTransformWidget(Set set);
		void RenderTagsTable(uint32_t* tagsInherited, uint32_t* tagsLocal, uint32_t* tagsGlobal, const char* const* tagLabels);
		void RenderCTagsWidget(Set set);
		void RenderQuadWidget(Renderer2D::Quad& quad, bool transparency);
	};
}