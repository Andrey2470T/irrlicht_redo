#pragma once

#include <cstdint>

namespace utils {

typedef uint8_t u8;
typedef int8_t s8;
typedef char c8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;
typedef float f32;
typedef double f64;

typedef vector2d<f32> v2f;
typedef vector2d<s32> v2i;
typedef vector3d<f32> v3f;
typedef vector3d<s32> v3i;

typedef CMatrix4<f32> mat4f;

typedef line2d<f32> line2f;
typedef line2d<s32> line2i;

typedef rect<f32> rectf;
typedef rect<s32> recti;

typedef plane3d<f32> plane3f;
typedef plane3d<s32> plane3i;
typedef aabbox3d<f32> aabb3f;
typedef aabbox3d<s32> aabb3i;
