#pragma once

#include "FE_pch.h"

namespace fe
{
	enum class SDPrimitive
	{
		None = 0,
		Bool,
		Int,
		UInt,
		Float,
		Double
	};

	enum class SDStructure
	{
		None = 0,
		Scalar,
		Vector,
		Matrix
	};

	enum class SDType
	{
		None = 0,
		Bool, Bool2, Bool3, Bool4,
		Int, Int2, Int3, Int4,
		UInt, UInt2, UInt3, UInt4,
		Float, Float2, Float3, Float4,
		Double, Double2, Double3, Double4,
		Mat2, Mat2x3, Mat2x4,
		Mat3x2, Mat3, Mat3x4,
		Mat4x2, Mat4x3, Mat4
	};

	SDPrimitive SDPrimitiveInType(SDType type);
	SDStructure SDStructureInType(SDType type);
	uint32_t SDSizeOfType(SDType type);
	SDType SDTypeOfMatrix(uint8_t rows, uint8_t columns);
	
	inline uint8_t RowsOfMatrix(SDType matrix)
	{
		if ((uint8_t)matrix > 29 || (uint8_t)matrix < 21) {
			FE_CORE_ASSERT(false, "It is not a matrix!");
			return 0;
		}

		return ((uint8_t)matrix / 3) - 5;
	}

	inline uint8_t ColumnsOfMatrix(SDType matrix)
	{
		if ((uint8_t)matrix > 29 || (uint8_t)matrix < 21) {
			FE_CORE_ASSERT(false, "It is not a matrix!");
			return 0;
		}

		return ((uint8_t)matrix % 3) + 2;
	}
}