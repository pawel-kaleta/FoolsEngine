#pragma once

#include "FoolsEngine\Events\Event.h"
#include "FoolsEngine\Renderer\3 - Representation\Camera.h"
#include "FoolsEngine\Renderer\3 - Representation\Transform.h"

namespace fe
{
	class EditorCameraController
	{
	public:
		EditorCameraController(float width, float hight);

		void OnUpdate();
		void OnEvent(Ref<Events::Event> event);

		float GetZoomLevel() const { return m_Camera.GetOrthographicZoom(); }
		float SetZoomLevel(float lvl) { m_Camera.SetOrthographicZoom(lvl); }
		void Resize(float width, float hight);
		
		      Camera&    GetCamera()          { return m_Camera; }
		const Camera&    GetCamera()    const { return m_Camera; }
		      Transform& GetTransform()       { return m_Transform; }
		const Transform& GetTransform() const { return m_Transform; }

		glm::vec3 GetDirectionUp() const;
		glm::vec3 GetDirectionRight() const;
		glm::vec3 GetDirectionForward() const;
		glm::quat GetOrientation() const;

	private:
		Camera m_Camera;
		Transform m_Transform;

		float m_RotationSpeed = 30.0f;
		float m_MoveSpeed = 1.0f;

		glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };
		glm::vec2 m_ViewportSize = { 1280.0f, 720.0f };

		void OnMouseScrolled(Ref<Events::MouseScrolledEvent> event);

		void Move(int horizontalDir, int verticalDir, int viewDir);
		void Rotate(int headingDir, int pitchDir, int bankDir);
		void Zoom(float delta);
	};
}