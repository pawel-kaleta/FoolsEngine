#include "FE_pch.h"

#include "ShaderData.h"

namespace fe
{
	namespace ShaderData
	{
		const char* TypesArray[] =
		{
			"None",
			"Bool", "Bool2", "Bool3", "Bool4",
			"Int", "Int2", "Int3", "Int4",
			"UInt", "UInt2", "UInt3", "UInt4",
			"Float", "Float2", "Float3", "Float4",
			"Double", "Double2", "Double3", "Double4",
			"Mat2", "Mat2x3", "Mat2x4",
			"Mat3x2", "Mat3", "Mat3x4",
			"Mat4x2", "Mat4x3", "Mat4"
		};
	}
}