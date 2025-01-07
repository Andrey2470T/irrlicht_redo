#pragma once

#include <cstddef>

namespace utils
{

extern size_t getSizeOfType(BasicType type) {
    switch (type) {
		case BasicType::UINT8:
			return sizeof(u8);
		case BasicType::UINT16:
			return sizeof(u16);
		case BasicType::UINT32:
			return sizeof(u32);
		case BasicType::UINT64:
			return sizeof(u64);
		case BasicType::CHAR:
			return sizeof(s8);
		case BasicType::SHORT:
			return sizeof(s16);
		case BasicType::INT:
			return sizeof(s32);
		case BasicType::LONG_INT:
			return sizeof(s64);
		case BasicType::FLOAT:
			return sizeof(f32);
		case BasicType::DOUBLE:
			return sizeof(f64);
		default:
			return 0;
	};
}

}
