#pragma once

#include "FoolsEngine\Events\Event.h"
#include "FoolsEngine\Scene\Component.h"

namespace fe
{
	class EditorCameraController
	{
	public:
		EditorCameraController(float aspectRatio);

		void OnUpdate();
		void OnEvent(Ref<Events::Event> event);

		float GetZoomLevel() const { return m_Camera.Zoom; }
		float SetZoomLevel(float lvl) { m_Camera.Zoom = lvl; }
		void Resize(float width, float hight);
		
		      CCamera&    GetCamera()          { return m_Camera; }
		const CCamera&    GetCamera()    const { return m_Camera; }
		      Transform& GetTransform()       { return m_Transform; }
		const Transform& GetTransform() const { return m_Transform; }
	private:
		CCamera m_Camera;
		Transform m_Transform;

		float m_TranslationSpeed = 0.5f;
		float m_RotationSpeed = 30.0f;

		void OnMouseScrolled(Ref<Events::MouseScrolledEvent> event);
	};
}