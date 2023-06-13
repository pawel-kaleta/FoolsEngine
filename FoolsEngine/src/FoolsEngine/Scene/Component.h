#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\quaternion.hpp>

namespace fe
{
	class Component
	{
	};

	struct CName : Component
	{
		CName(const std::string& name)
			: Name(name) {}
		CName(const CName&) = default;
		CName() = default;

		std::string Name = std::string();

		operator       std::string& () { if (Name.empty()) Name = "Set"; return Name; }
		operator const std::string& () { if (Name.empty()) Name = "Set"; return Name; }
	};

	struct CTags
	{
		uint32_t TagBitFlags = 0;

		CTags() = default;
		CTags(const CTags&) = default;
		CTags(uint32_t tags)
			: TagBitFlags(tags) {};

		operator       uint32_t& ()       { return TagBitFlags; }
		operator const uint32_t& () const { return TagBitFlags; }

		bool Contains(uint32_t tag)
		{
			return TagBitFlags & tag;
		}
		void Remove(uint32_t tag)
		{
			TagBitFlags &= ~tag;
		}
		void Add(uint32_t tag)
		{
			TagBitFlags |= tag;
		}
	};

	struct CCommonTags : CTags
	{
		//You can add your own tag to definition and/or make your own component with your own list
		enum CommonTags : uint32_t
		{
			Error = BIT_FLAG(0),
			Root = BIT_FLAG(1)
		};
	};

	struct CTransform : Component
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		CTransform() = default;
		CTransform(const CTransform&) = default;
		CTransform(const glm::vec3& translation, const glm::vec3& rotation = {0.0f, 0.0f, 0.0f}, const glm::vec3& scale = { 1.0f, 1.0f, 1.0f })
			: Position(translation), Rotation(rotation), Scale(scale) {};
		
		glm::mat4 GetTransform() const
		{
			auto trans = glm::translate(glm::mat4(1.0f), Position);
			auto rot = glm::toMat4(glm::quat(glm::radians(Rotation)));
			auto scale = glm::scale(glm::mat4(1.0f), Scale);

			return trans * rot * scale;
		}

		operator       glm::mat4 ()       { return GetTransform(); }
		operator const glm::mat4 () const { return GetTransform(); }
	};

	struct CCamera : Component
	{
		enum CameraMode
		{
			Orthographic,
			Isometric,
			Perspective
		};

		CameraMode Mode = CameraMode::Orthographic;
		float AspectRatio;
		float Zoom;
		float NearClip;
		float FarClip;

		CCamera() = default;
		CCamera(const CCamera& other) = default;
		CCamera(CameraMode mode, float aspectRatio, float zoom = 1.0f, float nearClip = -1.0f, float farClip = 1.0f)
			: Mode(mode), AspectRatio(aspectRatio), Zoom(zoom), NearClip(nearClip), FarClip(farClip) {};

		void SetViewportSize(uint32_t width, uint32_t hight) { AspectRatio = (float)width / (float)hight; };

		glm::mat4 ProjectionMatrix() { return CalculateProjection(); };
		operator       glm::mat4()       { return CalculateProjection(); }
		operator const glm::mat4() const { return CalculateProjection(); }

	private:
		glm::mat4 CalculateProjection() const
		{
			float top    = Zoom *  0.5f;
			float bottom = Zoom * -0.5f;
			float right  = AspectRatio * top;
			float left   = AspectRatio * bottom;

			switch (Mode)
			{
			case Orthographic:
				return glm::ortho(left, right, bottom, top, NearClip, FarClip);
			case Isometric:
				FE_CORE_ASSERT(false, "Isometric camera mode not supported yet");
			case Perspective:
				FE_CORE_ASSERT(false, "Perspective camera mode not supported yet");
			}

			return glm::ortho(left, right, bottom, top, NearClip, FarClip);
		};
	};

}