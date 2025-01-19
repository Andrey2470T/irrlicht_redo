#pragma once

#include <cstdint>
#include <limits>

#define T_MIN(t) \
	std::numeric_limits<t>::min()

#define T_MAX(t) \
	std::numeric_limits<t>::max()

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

enum class BasicType
{
	UINT8,
	UINT16,
	UINT32,
	UINT64,

	CHAR,
	SHORT,
	INT,
	LONG_INT,

	FLOAT,
	DOUBLE,
	COUNT
};
