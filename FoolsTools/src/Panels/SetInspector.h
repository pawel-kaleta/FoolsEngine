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

		static void AddComponentPopupMenu(Set set);

		static void DrawCNameWidget(Set set);

		static void DrawCTransformWidget(Set set);

		static void DrawTagsTable(uint32_t* tagsInherited, uint32_t* tagsLocal, uint32_t* tagsGlobal, const char* const* tagLabels);
		static void DrawCTagsWidget(Set set);

		static void DrawQuadWidget(Renderer2D::Quad& quad, bool transparency);
		static void DrawCSpriteWidget(Renderer2D::CSprite& sprite, Set set);
		static void DrawCTileWidget(Renderer2D::CTile& tile, Set set);
		 
		static void DrawCCameraWidget(CCamera& camera, Set set);
	};
}