#pragma once

#include "FoolsEngine/Foundation/Utils/DeclareEnum.h"
#include "FoolsEngine/Foundation/Memory/Splice.h"

#include <glm/glm.hpp>

namespace fe::GAPI::Data
{
	using std140_bool = U32;
	struct alignas( 8) std140_bvec2 : Array< std140_bool, 2 > {};
	struct alignas(16) std140_bvec3 : Array< std140_bool, 3 > {};
	struct alignas(16) std140_bvec4 : Array< std140_bool, 4 > {};

	using std140_int = S32;
	struct alignas( 8) std140_ivec2 : Array< std140_int, 2 > {};
	struct alignas(16) std140_ivec3 : Array< std140_int, 3 > {};
	struct alignas(16) std140_ivec4 : Array< std140_int, 4 > {};

	using std140_uint = U32;
	struct alignas( 8) std140_uvec2 : Array< std140_uint, 2 > {};
	struct alignas(16) std140_uvec3 : Array< std140_uint, 3 > {};
	struct alignas(16) std140_uvec4 : Array< std140_uint, 4 > {};

	using std140_float = F32;
	struct alignas( 8) std140_vec2 : Array< std140_float, 2 > {};
	struct alignas(16) std140_vec3 : Array< std140_float, 3 > {};
	struct alignas(16) std140_vec4 : Array< std140_float, 4 > {};

	using std140_double = F64;
	struct alignas(16) std140_dvec2 : Array< std140_double, 2 > {};
	struct alignas(32) std140_dvec3 : Array< std140_double, 3 > {};
	struct alignas(32) std140_dvec4 : Array< std140_double, 4 > {};

	template <typename T, UInt count>
	struct std140_Array
	{
		struct __declspec(align((sizeof(T) + (15)) & ~(15))) ElementType : T{};
		Array<ElementType, count> Data;
	};

	using std140_mat2x2 = std140_Array<std140_vec2, 2>;
	using std140_mat2x3 = std140_Array<std140_vec3, 2>;
	using std140_mat2x4 = std140_Array<std140_vec4, 2>;
	using std140_mat3x2 = std140_Array<std140_vec2, 3>;
	using std140_mat3x3 = std140_Array<std140_vec3, 3>;
	using std140_mat3x4 = std140_Array<std140_vec4, 3>;
	using std140_mat4x2 = std140_Array<std140_vec2, 4>;
	using std140_mat4x3 = std140_Array<std140_vec3, 4>;
	using std140_mat4x4 = std140_Array<std140_vec4, 4>;
}