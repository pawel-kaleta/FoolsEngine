#pragma once

#include "FoolsEngine\Utils\DeclareEnum.h"

namespace fe
{
	namespace ShaderData
	{
		FE_DECLARE_ENUM(Primitive,
			None, Bool, Int, UInt, Float, Double);

		FE_DECLARE_ENUM(Structure,
			None, Scalar, Vector, Matrix);

		FE_DECLARE_ENUM(Type,
			None,
			Bool, Bool2, Bool3, Bool4,
			Int, Int2, Int3, Int4,
			UInt, UInt2, UInt3, UInt4,
			Float, Float2, Float3, Float4,
			Double, Double2, Double3, Double4,
			Mat2, Mat2x3, Mat2x4,
			Mat3x2, Mat3, Mat3x4,
			Mat4x2, Mat4x3, Mat4
		);

		constexpr Primitive PrimitiveInType(Type type)
		{
			if (type.ToInt() > 29 || type.ToInt() <= 0) {
				FE_CORE_ASSERT(false, "Uknown Shader Data Type!");
				return Primitive::None;
			}

			if (type.ToInt() > 20) return Primitive::Float;
			switch ((type.ToInt() - 1) / 4)
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
			if (type.ToInt() > 29 || type.ToInt() <= 0) {
				FE_CORE_ASSERT(false, "Uknown Shader Data Type!");
				return Structure::None;
			}

			if (type.ToInt() > 20) return Structure::Matrix;
			switch ((type.ToInt() - 1) % 4)
			{
			case 0: return Structure::Scalar;
			case 1: return Structure::Vector;
			case 2: return Structure::Vector;
			case 3: return Structure::Vector;
			}

			FE_CORE_ASSERT(false, "Failed to detect ShaderData::Structure in ShaderData::Type.");
			return Structure::None;
		}

		constexpr uint32_t CountInVector(Type type)
		{
			if (type.ToInt() < 1 || type.ToInt() >= 21) {
				FE_CORE_ASSERT(false, "It is not vector!");
				return 0;
			}

			return type.ToInt() % 4;
		}

		constexpr Type TypeOfMatrix(uint32_t rows, uint32_t columns)
		{
			uint32_t result = 20;
			result += columns - 1;
			result += (rows - 2) * 3;
			Type type = Type::None;
			type.FromInt(result);
			return type;
		}

		constexpr uint32_t RowsOfMatrix(Type matrix)
		{
#ifdef FE_INTERNAL_BUILD
			if (matrix.ToInt() > 29 || matrix.ToInt() < 21) {
				FE_CORE_ASSERT(false, "It is not a matrix!");
				return 0;
			}
#endif // FE_INTERNAL_BUILD

			return (matrix.ToInt() / 3) - 5;
		}

		constexpr uint32_t ColumnsOfMatrix(Type matrix)
		{
#ifdef FE_INTERNAL_BUILD
			if (matrix.ToInt() > 29 || matrix.ToInt() < 21) {
				FE_CORE_ASSERT(false, "It is not a matrix!");
				return 0;
			}
#endif // FE_INTERNAL_BUILD

			return (matrix.ToInt() % 3) + 2;
		}
		
		constexpr size_t SizeOfType(Type type)
		{
#ifdef FE_INTERNAL_BUILD
			if (type.ToInt() > 29 || type.ToInt() <= 0) {
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

			size_t count = 0;
			if (type.ToInt() < 21) // not a matrix
			{
				count = ((type.ToInt() - 1) % 4) + 1;
				bool ifDouble = type.ToInt() >= 17;
				return count * 4 * (1 + (int)ifDouble);
			}

			return RowsOfMatrix(type) * ColumnsOfMatrix(type) * 4;
		}

		constexpr size_t SizeOfPrimitive(Primitive primitive)
		{
			switch (primitive.Value)
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