#pragma once

#include "FoolsEngine/Foundation/Utils/DeclareEnum.h"
#include "FoolsEngine/Foundation/Containers/Splice.h"

#include <glm/glm.hpp>

namespace fe::Render::GAPI
{
	using std140_bool = U32;
	struct alignas( 8) std140_bvec2 : glm::uvec2 {};
	struct alignas(16) std140_bvec3 : glm::uvec3 {};
	struct alignas(16) std140_bvec4 : glm::uvec4 {};

	using std140_int = S32;
	struct alignas( 8) std140_ivec2 : glm::ivec2 {};
	struct alignas(16) std140_ivec3 : glm::ivec3 {};
	struct alignas(16) std140_ivec4 : glm::ivec4 {};

	using std140_uint = U32;
	struct alignas( 8) std140_uvec2 : glm::uvec2 {};
	struct alignas(16) std140_uvec3 : glm::uvec3 {};
	struct alignas(16) std140_uvec4 : glm::uvec4 {};

	using std140_float = F32;
	struct alignas( 8) std140_vec2 : glm::vec2 {};
	struct alignas(16) std140_vec3 : glm::vec3 {};
	struct alignas(16) std140_vec4 : glm::vec4 {};

	using std140_double = F64;
	struct alignas(16) std140_dvec2 : glm::dvec2 {};
	struct alignas(32) std140_dvec3 : glm::dvec3 {};
	struct alignas(32) std140_dvec4 : glm::dvec4 {};

	template <typename T, UInt count>
	struct std140_Array
	{
		struct alignas((sizeof(T) + 15) & ~15) ElementType : T {};
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

	struct DrawParams
	{
		std140_uint mPrimitiveCount = 0;
		// instance count
		std140_uint mIgnoreThis = 1;
		// assumes index buffer is an array of indices, it's not offset from begining of that buffer in bytes, its an index within array of indices
		std140_uint mFirstIndex = 0;
		// base vertex
		std140_int mIgnoreThis2 = 0;
		// base instance
		std140_uint mIgnoreThis3 = 0;

		DrawParams& Init() { new (this) DrawParams(); return *this; }
	};
}