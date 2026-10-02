#pragma once

#include "FoolsEngine/Foundation/Utils/DeclareEnum.h"
#include "FoolsEngine/Renderer/1 - GAPI/Data.h"

#include <glm/glm.hpp>

namespace fe::Render::Representation
{
	struct Light
	{
		FE_DECLARE_ENUM(Type, Directional, Spot, Point);

		virtual Type GetType() = 0;
	};

	struct DirectionalLight final : Light
	{
		struct alignas(16) GPUStruct 
		{
			GAPI::std140_float Direction_x;
			GAPI::std140_float Direction_y;
			GAPI::std140_float Direction_z;
			GAPI::std140_float Intensity;
			GAPI::std140_float Color_r;
			GAPI::std140_float Color_g;
			GAPI::std140_float Color_b;
			GAPI::std140_float zPadding_1;
		};

		glm::vec3 Direction;
		glm::vec3 Color;
		float Intensity;

		Type GetTypeStatic() { return Type::Directional; }
		virtual Type GetType() override final { return Type::Directional; }

		GPUStruct GetGPUStruct()
		{
			return {
				.Direction_x = Direction.x,
				.Direction_y = Direction.y,
				.Direction_z = Direction.z,
				.Intensity = Intensity,
				.Color_r = Color.r,
				.Color_g = Color.g,
				.Color_b = Color.b
			};
		}
	};

	struct PointLight final : Light
	{
		struct alignas(16) GPUStruct
		{
			GAPI::Data::std140_float Position_x;
			GAPI::Data::std140_float Position_y;
			GAPI::Data::std140_float Position_z;
			GAPI::Data::std140_float Intensity;
			GAPI::Data::std140_float Color_r;
			GAPI::Data::std140_float Color_g;
			GAPI::Data::std140_float Color_b;
			GAPI::Data::std140_float Range;
		};

		glm::vec3 Position;
		glm::vec3 Color;
		float Intensity;
		float Range;

		Type GetTypeStatic() { return Type::Point; }
		virtual Type GetType() override final { return Type::Point; }

		GPUStruct GetGPUStruct()
		{
			return {
				.Position_x = Position.x,
				.Position_y = Position.y,
				.Position_z = Position.z,
				.Intensity = Intensity,
				.Color_r = Color.r,
				.Color_g = Color.g,
				.Color_b = Color.b,
				.Range = Range
			};
		}
	};
}