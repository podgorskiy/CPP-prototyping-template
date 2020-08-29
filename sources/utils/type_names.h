#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <tuple>

namespace misc
{
	template<typename T>
	const char* GetTypeName();
	template<> inline const char* GetTypeName<bool>() { return "bool"; }
	template<> inline const char* GetTypeName<int8_t>() { return "int8"; }
	template<> inline const char* GetTypeName<uint8_t>() { return "uint8"; }
	template<> inline const char* GetTypeName<char>() { return "char"; }
	template<> inline const char* GetTypeName<int32_t>() { return "int32"; }
	template<> inline const char* GetTypeName<uint32_t>() { return "uint32"; }
	template<> inline const char* GetTypeName<float>() { return "float"; }
	template<> inline const char* GetTypeName<double>() { return "double"; }
	template<> inline const char* GetTypeName<int64_t>() { return "int64"; }
	template<> inline const char* GetTypeName<uint64_t>() { return "uint64"; }
	template<> inline const char* GetTypeName<glm::vec2>() { return "vec2"; }
	template<> inline const char* GetTypeName<glm::vec3>() { return "vec3"; }
	template<> inline const char* GetTypeName<glm::vec4>() { return "vec4"; }
	template<> inline const char* GetTypeName<glm::bvec2>() { return "bvec2"; }
	template<> inline const char* GetTypeName<glm::bvec3>() { return "bvec3"; }
	template<> inline const char* GetTypeName<glm::bvec4>() { return "bvec4"; }
	template<> inline const char* GetTypeName<glm::ivec2>() { return "ivec2"; }
	template<> inline const char* GetTypeName<glm::ivec3>() { return "ivec3"; }
	template<> inline const char* GetTypeName<glm::ivec4>() { return "ivec4"; }
	template<> inline const char* GetTypeName<glm::mat2>() { return "mat2"; }
	template<> inline const char* GetTypeName<glm::mat3>() { return "mat3"; }
	template<> inline const char* GetTypeName<glm::mat4>() { return "mat4"; }
	template<> inline const char* GetTypeName<glm::mat2x3>() { return "mat2x3"; }
	template<> inline const char* GetTypeName<glm::mat3x2>() { return "mat3x2"; }
	template<> inline const char* GetTypeName<glm::mat3x4>() { return "mat3x4"; }
	template<> inline const char* GetTypeName<glm::mat4x3>() { return "mat4x3"; }
}
