#include "FE_pch.h"

#include "ShaderData.h"

namespace fe
{
	SDPrimitive SDPrimitiveInType(SDType type)
	{
		if ((uint8_t)type > 29 || (uint8_t)type <= 0) {
			FE_CORE_ASSERT(false, "Uknown Shader Data Type!");
			return SDPrimitive::None;
		}

		if ((uint8_t)type > 20) return SDPrimitive::Float;
		switch (((uint8_t)type - 1) / 4)
		{
		case 0: return SDPrimitive::Bool;
		case 1: return SDPrimitive::Int;
		case 2: return SDPrimitive::UInt;
		case 3: return SDPrimitive::Float;
		case 4: return SDPrimitive::Double;
		}

		FE_CORE_ASSERT(false, "Failed to detect SDPrimitive in SDType.");
		return SDPrimitive::None;
	}

	SDStructure SDStructureInType(SDType type)
	{
		if ((uint8_t)type > 29 || (uint8_t)type <= 0) {
			FE_CORE_ASSERT(false, "Uknown Shader Data Type!");
			return SDStructure::None;
		}

		if ((uint8_t)type > 20) return SDStructure::Matrix;
		switch (((uint8_t)type - 1) % 4)
		{
		case 0: return SDStructure::Scalar;
		case 1: return SDStructure::Vector;
		case 2: return SDStructure::Vector;
		case 3: return SDStructure::Vector;
		}

		FE_CORE_ASSERT(false, "Failed to detect SDStructure in SDType.");
		return SDStructure::None;
	}

	uint32_t SDSizeOfType(SDType type)
	{
		if ((uint8_t)type > 29 || (uint8_t)type <= 0) {
			FE_CORE_ASSERT(false, "Uknown Shader Data Type!");
			return 0;
		}

		/*const static uint32_t SDSizeOfTypeLookupTable[] = {
			0,
			4, 4*2, 4*3, 4*4,
			4, 4*2, 4*3, 4*4,
			4, 4*2, 4*3, 4*4,
			4, 4*2, 4*3, 4*4,
			8, 8*2, 8*3, 8*4,
			4*2*2, 4*2*3, 4*2*4,
			4*3*2, 4*3*3, 4*3*4,
			4*4*2, 4*4*3, 4*4*4
		};

		return SDSizeOfTypeLookupTable[(int)type];*/

		static uint32_t count = 0;
		if ((uint8_t)type < 21)
		{
			count = (((uint8_t)type - 1) % 4) + 1;
			if ((uint8_t)type < 17)
				return count * 4;
			return count * 8;
		}

		return RowsOfMatrix(type) * ColumnsOfMatrix(type) * 4;
	}

	/*const SDType SDTypeOfMatrixLookupTable[3][3] =	{
		{ SDType::Mat2,   SDType::Mat2x3, SDType::Mat2x4 },
		{ SDType::Mat3x2, SDType::Mat3,   SDType::Mat3x4 },
		{ SDType::Mat4x2, SDType::Mat4x3, SDType::Mat4   }
	};*/

	SDType SDTypeOfMatrix(uint8_t rows, uint8_t columns)
	{
		static uint8_t type = 0;
		type = 20;
		type += columns - 1;
		type += (rows - 2) * 3;
		return (SDType)type;
	}

	 

}