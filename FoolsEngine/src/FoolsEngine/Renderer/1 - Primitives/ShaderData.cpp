#include "FE_pch.h"

#include "ShaderData.h"

namespace fe
{
	namespace ShaderData
	{
		Primitive PrimitiveInType(Type type)
		{
			if ((uint8_t)type > 29 || (uint8_t)type <= 0) {
				FE_CORE_ASSERT(false, "Uknown Shader Data Type!");
				return Primitive::None;
			}

			if ((uint8_t)type > 20) return Primitive::Float;
			switch (((uint8_t)type - 1) / 4)
			{
			case 0: return Primitive::Bool;
			case 1: return Primitive::Int;
			case 2: return Primitive::UInt;
			case 3: return Primitive::Float;
			case 4: return Primitive::Double;
			}

			FE_CORE_ASSERT(false, "Failed to detect SDPrimitive in SDType.");
			return Primitive::None;
		}

		Structure StructureInType(Type type)
		{
			if ((uint8_t)type > 29 || (uint8_t)type <= 0) {
				FE_CORE_ASSERT(false, "Uknown Shader Data Type!");
				return Structure::None;
			}

			if ((uint8_t)type > 20) return Structure::Matrix;
			switch (((uint8_t)type - 1) % 4)
			{
			case 0: return Structure::Scalar;
			case 1: return Structure::Vector;
			case 2: return Structure::Vector;
			case 3: return Structure::Vector;
			}

			FE_CORE_ASSERT(false, "Failed to detect SDStructure in SDType.");
			return Structure::None;
		}

		uint32_t SizeOfType(Type type)
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

			return SDSizeOfTypeLookupTable[type];*/

			static int count = 0;
			if ((int)type < 21)
			{
				count = (((int)type - 1) % 4) + 1;
				bool ifDouble = (int)type >= 17;
				return count * 4 * (1+(int)ifDouble);
			}

			return RowsOfMatrix(type) * ColumnsOfMatrix(type) * 4;
		}


		Type TypeOfMatrix(uint8_t rows, uint8_t columns)
		{
			/*const static SDType SDTypeOfMatrixLookupTable[3][3] =	{
				{ SDType::Mat2,   SDType::Mat2x3, SDType::Mat2x4 },
				{ SDType::Mat3x2, SDType::Mat3,   SDType::Mat3x4 },
				{ SDType::Mat4x2, SDType::Mat4x3, SDType::Mat4   }
			};
			
			return SDTypeOfMatrixLookupTable[rows-1][columns-1];*/


			static uint8_t type = 0;
			type = 20;
			type += columns - 1;
			type += (rows - 2) * 3;
			return (Type)type;
		}
	}
}