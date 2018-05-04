#pragma once

#include "monopp/mono_type_conversion.h"

namespace mono
{
namespace managed_interface
{

#define add_mono_type_converter(cpp_type, mono_type)                                                         \
	template <>                                                                                              \
	struct convert_mono_type<cpp_type>                                                                       \
	{                                                                                                        \
		using mono_type_name = mono_type;                                                                    \
                                                                                                             \
		static auto to_mono(mono_assembly& assembly, cpp_type val) -> mono_type                              \
		{                                                                                                    \
			(void)assembly;                                                                                  \
			return managed_interface::converter::convert<mono_type>(val);                                               \
		}                                                                                                    \
                                                                                                             \
		static auto from_mono(mono_type val) -> cpp_type                                                     \
		{                                                                                                    \
			return managed_interface::converter::convert<cpp_type>(val);                                               \
		}                                                                                                    \
	}

struct converter
{
	template <typename dst_t, typename src_t>
	static dst_t convert(const src_t& obj)
    {
        static_assert(std::is_same<dst_t, src_t>::value, 
                      "Please specialize convert function.");
    }

    
	// From Mono
	//	static auto convert(const rect& r) -> frect_t;
	//	static auto convert(const vector2f& vec) -> math::vec2;
	//	static auto convert(const vector3f& vec) -> math::vec3;
	//	static auto convert(const quaternion& quat) -> math::quat;
	//	static auto convert(const matrix4x4& mat) -> math::mat4x4;

	//	// To Mono
	//	static auto convert(const frect_t& r) -> rect;
	//	static auto convert(const glm::vec2& vec) -> vector2f;
	//	static auto convert(const glm::vec3& vec) -> vector3f;
	//	static auto convert(const glm::quat& quat) -> quaternion;
	//	static auto convert(const glm::mat4x4& m) -> matrix4x4;
};

///////////////////////////////////////////////////////////////////////////////

// inline auto converter::convert(const rect& r) -> frect_t
//{
//	return {r.left, r.top, r.right, r.bottom};
//}

// inline auto converter::convert(const vector2f& vec) -> math::vec2
//{
//	return {vec.x, vec.y};
//}

// inline auto converter::convert(const vector3f& vec) -> math::vec3
//{
//	return {vec.x, vec.y, vec.z};
//}

// inline auto converter::convert(const quaternion& quat) -> math::quat
//{
//	return {quat.w, quat.x, quat.y, quat.z};
//}

// inline auto converter::convert(const matrix4x4& mat) -> math::mat4x4
//{
//	math::mat4x4 m;
//	m[0][0] = mat.m00;
//	m[0][1] = mat.m01;
//	m[0][2] = mat.m02;
//	m[0][3] = mat.m03;

//	m[1][0] = mat.m10;
//	m[1][1] = mat.m11;
//	m[1][2] = mat.m12;
//	m[1][3] = mat.m13;

//	m[2][0] = mat.m20;
//	m[2][1] = mat.m21;
//	m[2][2] = mat.m22;
//	m[2][3] = mat.m23;

//	m[3][0] = mat.m30;
//	m[3][1] = mat.m31;
//	m[3][2] = mat.m32;
//	m[3][3] = mat.m33;
//	return m;
//}

/////////////////////////////////////////////////////////////////////////////////

// inline auto converter::convert(const frect_t& r) -> rect
//{
//	return {r.left, r.top, r.right, r.bottom};
//}

// inline auto converter::convert(const math::vec2& vec) -> vector2f
//{
//	return {vec.x, vec.y};
//}

// inline auto converter::convert(const math::vec3& vec) -> vector3f
//{
//	return {vec.x, vec.y, vec.z};
//}

// inline auto converter::convert(const math::quat& quat) -> quaternion
//{
//	return {quat.w, quat.x, quat.y, quat.z};
//}

// inline auto converter::convert(const math::mat4x4& m) -> matrix4x4
//{
//	return {m[0][0], m[0][1], m[0][2], m[0][3], m[1][0], m[1][1], m[1][2], m[1][3],
//			m[2][0], m[2][1], m[2][2], m[2][3], m[3][0], m[3][1], m[3][2], m[3][3]};
//}


struct B
{
    
};

} // namespace managed_interface
struct A
{
    
};


namespace managed_interface
{
template<>
inline auto converter::convert<A, B>(const B& r) -> A
{
    return {};
}

template<>
inline auto converter::convert<B, A>(const A& r) -> B
{
    return {};
}
}

add_mono_type_converter(A, managed_interface::B);

} // namespace mono
