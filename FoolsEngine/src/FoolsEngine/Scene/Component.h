#pragma once

#include "FoolsEngine\Renderer\OrthographicCamera.h"

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

	//You can add your own tag to definition
	enum CommonTags
	{
		Error = 0,
		Root = 1
	};

	struct CCommonTags
	{
		uint32_t Tags = 0;

		CCommonTags() = default;
		CCommonTags(CommonTags tag)
			: Tags(BIT_FLAG(tag)) {}
		CCommonTags(uint32_t tags)
			: Tags(tags) {}

		operator       uint32_t& ()       { return Tags; }
		operator const uint32_t& () const { return Tags; }

		bool Contains(CommonTags tag)
		{
			return Tags & tag;
		}
		void Remove(CommonTags tag)
		{
			Tags &= ~tag;
		}
		void Add(CommonTags tag)
		{
			Tags |= tag;
		}
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
			auto rot = glm::toMat4(glm::quat(Rotation));
			auto scale = glm::scale(glm::mat4(1.0f), Scale);
			return trans * rot * scale;
		}
	};

	struct CCamera : Component
	{
		glm::mat4 ProjectionMatrix = glm::mat4(1.0f);

		CCamera() = default;
		CCamera(const CCamera& other) = default;
		CCamera(const glm::mat4& projection)
			: ProjectionMatrix(projection) {};
		CCamera(float left, float right, float bottom, float top)
			: ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)) {};
	};
}