#pragma once

namespace fe
{
	namespace ShaderData
	{
		enum class Primitive
		{
			None = 0,
			Bool,
			Int,
			UInt,
			Float,
			Double
		};
		enum class Structure
		{
			None = 0,
			Scalar,
			Vector,
			Matrix
		};
		enum class Type
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

		const char* TypesArray[];

		constexpr Primitive PrimitiveInType(Type type)
		{
			if ((uint32_t)type > 29 || (uint32_t)type <= 0) {
				FE_CORE_ASSERT(false, "Uknown Shader Data Type!");
				return Primitive::None;
			}

			if ((uint32_t)type > 20) return Primitive::Float;
			switch (((uint32_t)type - 1) / 4)
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

		constexpr Structure StructureInType(Type type)
		{
			if ((uint32_t)type > 29 || (uint32_t)type <= 0) {
				FE_CORE_ASSERT(false, "Uknown Shader Data Type!");
				return Structure::None;
			}

			if ((uint32_t)type > 20) return Structure::Matrix;
			switch (((uint32_t)type - 1) % 4)
			{
			case 0: return Structure::Scalar;
			case 1: return Structure::Vector;
			case 2: return Structure::Vector;
			case 3: return Structure::Vector;
			}

			FE_CORE_ASSERT(false, "Failed to detect SDStructure in SDType.");
			return Structure::None;
		}

		constexpr Type TypeOfMatrix(uint32_t rows, uint32_t columns)
		{
			/*const static uint8_t SDTypeOfMatrixLookupTable[3][3] = {
			{ SDType::Mat2,   SDType::Mat2x3, SDType::Mat2x4 },
			{ SDType::Mat3x2, SDType::Mat3,   SDType::Mat3x4 },
			{ SDType::Mat4x2, SDType::Mat4x3, SDType::Mat4   }
			};

			return (Type)SDTypeOfMatrixLookupTable[rows-1][columns-1];*/


			uint32_t type = 0;
			type = 20;
			type += columns - 1;
			type += (rows - 2) * 3;
			return (Type)type;
		}

		constexpr uint32_t RowsOfMatrix(Type matrix)
		{
#ifdef FE_INTERNAL_BUILD
			if ((uint32_t)matrix > 29 || (uint32_t)matrix < 21) {
				FE_CORE_ASSERT(false, "It is not a matrix!");
				return 0;
			}
#endif // FE_INTERNAL_BUILD

			return ((uint32_t)matrix / 3) - 5;
		}

		constexpr uint32_t ColumnsOfMatrix(Type matrix)
		{
#ifdef FE_INTERNAL_BUILD
			if ((uint32_t)matrix > 29 || (uint32_t)matrix < 21) {
				FE_CORE_ASSERT(false, "It is not a matrix!");
				return 0;
			}
#endif // FE_INTERNAL_BUILD

			return ((uint32_t)matrix % 3) + 2;
		}
		
		constexpr uint32_t SizeOfType(Type type)
		{
#ifdef FE_INTERNAL_BUILD
			if ((uint32_t)type > 29 || (uint32_t)type <= 0) {
				FE_CORE_ASSERT(false, "Uknown Shader Data Type!");
				return 0;
			}
#endif // FE_INTERNAL_BUILD

			/*const static uint8_t SDSizeOfTypeLookupTable[] = {
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

			uint32_t count = 0;
			if ((uint32_t)type < 21) // not a matrix
			{
				count = (((uint32_t)type - 1) % 4) + 1;
				bool ifDouble = (uint32_t)type >= 17;
				return count * 4 * (1 + (uint32_t)ifDouble);
			}

			return RowsOfMatrix(type) * ColumnsOfMatrix(type) * 4;
		}

		constexpr uint32_t SizeOfPrimitive(Primitive primitive)
		{
			switch (primitive)
			{
			case Primitive::None:
				return 0;
			case Primitive::Double:
				return 8;
			default:
				return 4;
			}
		}
	}
}