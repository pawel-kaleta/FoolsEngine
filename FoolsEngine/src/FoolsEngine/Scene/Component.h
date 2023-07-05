#pragma once

#include "ECS.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\quaternion.hpp>

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

	struct CHierarchicalBase : ComponentBase {};

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
		enum CameraMode
		{
			Orthographic,
			Isometric,
			Perspective
		};

		CameraMode Mode = CameraMode::Orthographic;
		float AspectRatio = 1280.0f / 720.0f;
		float Zoom = 1.0f;
		float NearClip = -1.0f;
		float FarClip = 1.0f;

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

	struct CDestroyFlag : ComponentBase
	{};
}