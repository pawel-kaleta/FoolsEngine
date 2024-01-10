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

		Primitive PrimitiveInType(Type type);
		Structure StructureInType(Type type);
		uint32_t SizeOfType(Type type);
		Type TypeOfMatrix(uint8_t rows, uint8_t columns);

		inline uint8_t RowsOfMatrix(Type matrix)
		{
			if ((uint8_t)matrix > 29 || (uint8_t)matrix < 21) {
				FE_CORE_ASSERT(false, "It is not a matrix!");
				return 0;
			}

			return ((uint8_t)matrix / 3) - 5;
		}
		inline uint8_t ColumnsOfMatrix(Type matrix)
		{
			if ((uint8_t)matrix > 29 || (uint8_t)matrix < 21) {
				FE_CORE_ASSERT(false, "It is not a matrix!");
				return 0;
			}

			return ((uint8_t)matrix % 3) + 2;
		}
	}
}