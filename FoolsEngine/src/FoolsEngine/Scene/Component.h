#pragma once

#include "ECS.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

namespace fe
{
	class Scene;
	
	struct ComponentBase {};

	struct CName : ComponentBase
	{
		CName(const std::string& name)
			: Name(name) {}

		std::string Name = std::string();

		operator       std::string& () { return Name; }
		operator const std::string& () { return Name; }
	};

	struct CHierarchyNode : ComponentBase
	{
		SetID Parent = RootID;

		uint32_t HierarchyLvl = 0; // = parent.HierarchyLvl + 1;
		
		SetID PreviousSibling = NullSetID;
		SetID NextSibling = NullSetID;

		uint32_t Children = 0;
		SetID FirstChild = NullSetID;
		SetID LastChild = NullSetID;
	};
	
	struct TagsBase
	{
		uint32_t TagBitFlags = 0;

		TagsBase() = default;
		TagsBase(uint32_t tags)
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

		TagsBase operator+ (const TagsBase& other) const
		{
			return TagsBase(this->TagBitFlags | other.TagBitFlags);
		}
		TagsBase operator- (const TagsBase& other) const
		{
			return TagsBase(this->TagBitFlags & ~other.TagBitFlags);
		}
		bool operator==(const TagsBase& other) const
		{
			return TagsBase(TagBitFlags == other.TagBitFlags);
		}

	};

	struct CommonTags : public TagsBase
	{
		enum List : uint32_t
		{
			Error = BIT_FLAG(0),
			Player = BIT_FLAG(1)
		};
	};
	struct InternalTags : public TagsBase
	{};
	struct Tags
	{
		CommonTags Common;
		InternalTags Internal;

		Tags operator+ (const Tags& other) const
		{
			Tags newTags;
			newTags.Common.TagBitFlags   = Common.TagBitFlags | other.Common.TagBitFlags;
			newTags.Internal.TagBitFlags = Internal.TagBitFlags | other.Internal.TagBitFlags;
			return newTags;
		}
		Tags operator- (const Tags& other) const
		{
			Tags newTags;
			(TagsBase)newTags.Common   = Common   - other.Common;
			(TagsBase)newTags.Internal = Internal - other.Internal;
			return newTags;
		}
		bool operator==(const Tags& other) const
		{
			return Common == other.Common && Internal == other.Internal;
		}
	};

	struct Transform
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		glm::mat4 GetTransform() const
		{
			auto trans = glm::translate(glm::mat4(1.0f), Position);
			auto rot = glm::toMat4(glm::quat(glm::radians(Rotation)));
			auto scale = glm::scale(glm::mat4(1.0f), Scale);
			return trans * rot * scale;
		}

		operator       glm::mat4()       { return GetTransform(); }
		operator const glm::mat4() const { return GetTransform(); }

		Transform operator+ (const Transform& other) const
		{
			Transform newTransform;
			newTransform.Position = this->Position + this->Scale * glm::rotate(glm::quat(glm::radians(this->Rotation)), other.Position);
			newTransform.Rotation = this->Rotation + other.Rotation;
			newTransform.Scale = this->Scale * other.Scale;
			return newTransform;
		}
		Transform operator- (const Transform& other) const
		{
			Transform newTransform;
			newTransform.Position = glm::rotate(glm::quat(-1.0f * other.Rotation), (this->Position - other.Position));
			newTransform.Rotation = this->Rotation - other.Rotation;
			newTransform.Scale = this->Scale / other.Scale;
			return newTransform;
		}
		bool operator==(const Transform& other) const
		{
			return Position == other.Position && Rotation == other.Rotation && Scale == other.Scale;
		}
	};

	struct CHierarchicalBase : ComponentBase
	{};

	template<typename DataStruct> //DataStruct needs to implement +,-,== operators
	struct CHierarchical : CHierarchicalBase
	{
		CHierarchical() = default;
		CHierarchical(const CHierarchical&) = default;

		DataStruct Global;
		DataStruct Local;

		operator const DataStruct() const { return Global; }
	};

	using CTransform = CHierarchical<Transform>;
	using CTags = CHierarchical<Tags>;

	struct CCamera : ComponentBase
	{
		enum ProjectionType
		{
			Orthographic,
			Perspective
		};

		CCamera() {	CalculateProjection(); }
		~CCamera() = default;

		glm::mat4 GetProjectionMatrix() const { return m_Projection; };
		operator       glm::mat4()       { return m_Projection; }
		operator const glm::mat4() const { return m_Projection; }

		void SetOrthographic(float zoom, float nearClip, float farClip);
		void SetPerspective(float verticalFOV, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t hight) { m_AspectRatio = (float)width / (float)hight; CalculateProjection(); }

		//radians
		float GetPerspectiveFOV() const { return m_PerspectiveFOV; }
		//radians
		void SetPerspectiveFOV(float FOV) { m_PerspectiveFOV = FOV; CalculateProjection(); }
		float GetPerspectiveNearClip() const { return m_PerspectiveNearClip; }
		void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNearClip = nearClip; CalculateProjection(); }
		float GetPerspectiveFarClip() const { return m_PerspectiveFarClip; }
		void SetPerspectiveFarClip(float farClip) { m_PerspectiveFarClip = farClip; CalculateProjection(); }

		float GetOrthographicZoom() const { return m_OrthographicZoom; }
		void SetOrthographicZoom(float zoom) { m_OrthographicZoom = zoom; CalculateProjection(); }
		float GetOrthographicNearClip() const { return m_OrthographicNearClip; }
		void SetOrthographicNearClip(float nearClip) { m_OrthographicNearClip = nearClip; CalculateProjection(); }
		float GetOrthographicFarClip() const { return m_OrthographicFarClip; }
		void SetOrthographicFarClip(float farClip) { m_OrthographicFarClip = farClip; CalculateProjection(); }

		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(ProjectionType type) { m_ProjectionType = type; CalculateProjection(); }

	private:
		glm::mat4 m_Projection = glm::mat4(1.0f);
		ProjectionType m_ProjectionType = ProjectionType::Perspective;
		float m_AspectRatio = 1280.0f / 720.0f;

		float m_PerspectiveFOV = glm::radians(45.0f);
		float m_PerspectiveNearClip = 0.01f;
		float m_PerspectiveFarClip = 1000.0f;

		float m_OrthographicZoom = 1.0f;
		float m_OrthographicNearClip = -1.0f;
		float m_OrthographicFarClip = 1.0f;

		void CalculateProjection();
	};

	struct CDestroyFlag : ComponentBase
	{};
}