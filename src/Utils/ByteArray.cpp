#include "ByteArray.h"
#include <SDL_endian.h>

namespace utils
{

void ByteArray::reallocate(u32 elemCount, u32 bytesCount)
{
    if (elemCount == count())
        return;
    elements.resize(elemCount);
    bytes.resize(bytesCount);
}

void ByteArray::extendBytes(const ByteArray *add_bytes, u32 offset, u32 count)
{
    u32 curElemCount = this->count();

    elements.reserve(elements.size()+count);

    for (u32 k = 0; k < count; k++)
        elements.push_back({BasicType::UINT8, 1, curElemCount+k});

    memcpy(bytes.data()+offset, (u8 *)add_bytes->data(), count);
}

void ByteArray::setUInt8(u8 elem, u32 n) 		{ setElement({BasicType::UINT8, 1}, &elem, n); }
void ByteArray::setUInt16(u16 elem, u32 n) 		{ setElement({BasicType::UINT16, 2}, &elem, n); }
void ByteArray::setUInt32(u32 elem, u32 n)		{ setElement({BasicType::UINT32, 4}, &elem, n); }
void ByteArray::setUInt64(u64 elem, u32 n)		{ setElement({BasicType::UINT64, 8}, &elem, n); }

void ByteArray::setChar(c8 elem, u32 n) 		{ setElement({BasicType::CHAR, 1}, &elem, n); }
void ByteArray::setShort(s16 elem, u32 n) 		{ setElement({BasicType::SHORT, 2}, &elem, n); }
void ByteArray::setInt(s32 elem, u32 n) 		{ setElement({BasicType::INT, 4}, &elem, n); }
void ByteArray::setLongInt(s64 elem, u32 n)     { setElement({BasicType::LONG_INT, 8}, &elem, n); }

void ByteArray::setFloat(f32 elem, u32 n) 		{ setElement({BasicType::FLOAT, 4}, &elem, n); }
void ByteArray::setDouble(f64 elem, u32 n)      { setElement({BasicType::DOUBLE, 8}, &elem, n); }

void ByteArray::setV2U(v2u elem, u32 n)
{
    setUInt32(elem.X, n);
    setUInt32(elem.Y, n + 1);
}

void ByteArray::setV2I(v2i elem, u32 n)
{
    setInt(elem.X, n);
    setInt(elem.Y, n + 1);
}

void ByteArray::setV2F(v2f elem, u32 n)
{
    setFloat(elem.X, n);
    setFloat(elem.Y, n + 1);
}

void ByteArray::setV3U(v3u elem, u32 n)
{
    setUInt32(elem.X, n);
    setUInt32(elem.Y, n + 1);
    setUInt32(elem.Z, n + 2);
}

void ByteArray::setV3I(v3i elem, u32 n)
{
    setInt(elem.X, n);
    setInt(elem.Y, n + 1);
    setInt(elem.Z, n + 2);
}

void ByteArray::setV3F(v3f elem, u32 n)
{
    setFloat(elem.X, n);
    setFloat(elem.Y, n + 1);
    setFloat(elem.Z, n + 2);
}

void ByteArray::setM4x4(const matrix4 &elem, u32 n)
{
    for (u8 i = 0; i < 16; i++)
        setFloat(elem[i], n+i);
}

//! Getters
// 'n' is a number of the byte array element
u8 ByteArray::getUInt8(u32 n) const
{
    return getElement(n)[0];
}
u16 ByteArray::getUInt16(u32 n) const
{
    auto elem = getElement(n);

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    u16 elem_u16 = elem[0];
    elem_u16 |= ((u16)elem[1] << 8);
#else
    u16 elem_u16 = elem[1];
    elem_u16 |= ((u16)elem[0] << 8);
#endif

    return elem_u16;
}
u32 ByteArray::getUInt32(u32 n) const
{
    auto elem = getElement(n);

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    u32 elem_u32 = elem[0];
    elem_u32 |= ((u32)elem[1] << 8);
    elem_u32 |= ((u32)elem[2] << 16);
    elem_u32 |= ((u32)elem[3] << 24);
#else
    u32 elem_u32 = elem[3];
    elem_u32 |= ((u32)elem[2] << 8);
    elem_u32 |= ((u32)elem[1] << 16);
    elem_u32 |= ((u32)elem[0] << 24);
#endif

    return elem_u32;
}
u64 ByteArray::getUInt64(u32 n) const
{
    auto elem = getElement(n);

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    u64 elem_u64 = elem[0];
    elem_u64 |= ((u64)elem[1] << 8);
    elem_u64 |= ((u64)elem[2] << 16);
    elem_u64 |= ((u64)elem[3] << 24);
    elem_u64 |= ((u64)elem[4] << 32);
    elem_u64 |= ((u64)elem[5] << 40);
    elem_u64 |= ((u64)elem[6] << 48);
    elem_u64 |= ((u64)elem[7] << 56);
#else
    u64 elem_u64 = elem[7];
    elem_u64 |= ((u64)elem[6] << 8);
    elem_u64 |= ((u64)elem[5] << 16);
    elem_u64 |= ((u64)elem[4] << 24);
    elem_u64 |= ((u64)elem[3] << 32);
    elem_u64 |= ((u64)elem[2] << 40);
    elem_u64 |= ((u64)elem[1] << 48);
    elem_u64 |= ((u64)elem[0] << 56);
#endif


    return elem_u64;
}

v2u ByteArray::getV2U(u32 n) const
{
    return v2u(getUInt32(n), getUInt32(n + 1));
}

v2i ByteArray::getV2I(u32 n) const
{
    return v2i(getInt(n), getInt(n + 1));
}

v2f ByteArray::getV2F(u32 n) const
{
    return v2f(getFloat(n), getFloat(n + 1));
}

v3u ByteArray::getV3U(u32 n) const
{
    return v3u(getUInt32(n), getUInt32(n + 1), getUInt32(n + 2));
}

v3i ByteArray::getV3I(u32 n) const
{
    return v3i(getInt(n), getInt(n + 1), getInt(n + 2));
}

v3f ByteArray::getV3F(u32 n) const
{
    return v3f(getFloat(n), getFloat(n + 1), getFloat(n + 2));
}

matrix4 ByteArray::getM4x4(u32 n) const
{
    matrix4 m;

    for (u8 i = 0; i < 16; i++)
        m[i] = getFloat(n+i);

    return m;
}

s8 ByteArray::getChar(u32 n) const
{
    return (s8)getUInt8(n);
}
s16 ByteArray::getShort(u32 n) const
{
    return (s16)getUInt16(n);
}
s32 ByteArray::getInt(u32 n) const
{
    return (s32)getUInt32(n);
}
s64 ByteArray::getLongInt(u32 n) const
{
    return (s64)getUInt64(n);
}
f32 ByteArray::getFloat(u32 n) const
{
    u32 v = getUInt32(n);
    return *(f32*)(&v);
}
f64 ByteArray::getDouble(u32 n) const
{
    u64 v = getUInt64(n);
    return *(f64*)(&v);
}

u32 ByteArray::countBytesBefore(u32 n) const
{
    if (n >= count()) {
		WarnStream << "ByteArray::countBytesBefore() Access to the element outside of the byte array\n";
		return 0;
	}

	u32 count = 0;
	for (u32 i = 0; i < n; i++)
		count += elements[i].bytes_count;

	return count;
}

std::vector<u8> ByteArray::getElement(u32 n) const
{
	if (n >= count()) {
		WarnStream << "ByteArray::getElement() Access to the element outside of the byte array\n";
		return {};
	}

	std::vector<u8> elem_bytes;

	auto &elem = elements[n];

	elem_bytes.resize(elem.bytes_count);
	for (u32 i = 0; i < elem.bytes_count; i++)
		elem_bytes[i] = bytes[elem.offset+i];

	return elem_bytes;
}

void ByteArray::setElement(ByteArrayElement &&elem, void *data, u32 n)
{
    if (n >= count()) {
        WarnStream << "ByteArray::setElement() Access to the element outside of the byte array\n";
        return;
    }

	std::vector<u8> elem_bytes;

	switch (elem.type) {
		case BasicType::UINT8:
		case BasicType::CHAR: {
			elem_bytes.push_back(*(u8*)(data));
			break;
		}
		case BasicType::UINT16:
		case BasicType::SHORT: {
			elem_bytes.resize(2);
			u16 elem_u16 = *(u16*)(data);
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
            elem_bytes[0] = (u8)(elem_u16 & 0xFF);
            elem_bytes[1] = (u8)(elem_u16 >> 8);
#else
            elem_bytes[0] = (u8)(elem_u16 >> 8);
            elem_bytes[1] = (u8)(elem_u16 & 0xFF);
#endif
			break;
		}
		case BasicType::UINT32:
		case BasicType::INT:
		case BasicType::FLOAT: {
			elem_bytes.resize(4);
            u32 elem_u32 = *(u32*)(data);
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
            elem_bytes[0] = (u8)(elem_u32 & 0xFF);
            elem_bytes[1] = (u8)(elem_u32 >> 8 & 0xFF);
            elem_bytes[2] = (u8)(elem_u32 >> 16 & 0xFF);
            elem_bytes[3] = (u8)(elem_u32 >> 24);
#else
            elem_bytes[0] = (u8)(elem_u32 >> 24);
			elem_bytes[1] = (u8)(elem_u32 >> 16 & 0xFF);
			elem_bytes[2] = (u8)(elem_u32 >> 8 & 0xFF);
            elem_bytes[3] = (u8)(elem_u32 & 0xFF);
#endif
			break;
		}
		case BasicType::UINT64:
		case BasicType::LONG_INT:
		case BasicType::DOUBLE: {
			elem_bytes.resize(8);
			u64 elem_u64 = *(u64*)(data);
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
            elem_bytes[0] = (u8)(elem_u64 & 0xFF);
            elem_bytes[1] = (u8)(elem_u64 >> 8 & 0xFF);
            elem_bytes[2] = (u8)(elem_u64 >> 16 & 0xFF);
            elem_bytes[3] = (u8)(elem_u64 >> 24 & 0xFF);
            elem_bytes[4] = (u8)(elem_u64 >> 32 & 0xFF);
            elem_bytes[5] = (u8)(elem_u64 >> 40 & 0xFF);
            elem_bytes[6] = (u8)(elem_u64 >> 48 & 0xFF);
            elem_bytes[7] = (u8)(elem_u64 >> 56);
#else
            elem_bytes[0] = (u8)(elem_u64 >> 56);
            elem_bytes[1] = (u8)(elem_u64 >> 48 & 0xFF);
            elem_bytes[2] = (u8)(elem_u64 >> 40 & 0xFF);
            elem_bytes[3] = (u8)(elem_u64 >> 32 & 0xFF);
            elem_bytes[4] = (u8)(elem_u64 >> 24 & 0xFF);
            elem_bytes[5] = (u8)(elem_u64 >> 16 & 0xFF);
            elem_bytes[6] = (u8)(elem_u64 >> 8 & 0xFF);
            elem_bytes[7] = (u8)(elem_u64 & 0xFF);
#endif
			break;
		}
		default:
			break;
	}

    auto &n_elem = elements.at(n);
    n_elem.type = elem.type;
    n_elem.bytes_count = elem.bytes_count;
    n_elem.offset = countBytesBefore(n);

	for (u32 i = 0; i < elem_bytes.size(); i++)
        bytes[n_elem.offset + i] = elem_bytes[i];
}

}
