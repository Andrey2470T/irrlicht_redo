#pragma once

#include "BasicIncludes.h"

class ByteArray
{
	struct ByteArrayElement
	{
		BasicType type;
		u32 bytes_count;
		u32 offset = 0;
	};

	//! Bytes storage
	std::vector<u8> bytes;
	//! Elements (base types) saving inside 'bytes'
	std::vector<ByteArrayElement> elements;
	//! Current position of the 'bytes' indicator (offset in bytes)
	u32 pos_i = 0;
public:
	ByteArray() = default;

	//! Constructor. 'count' is an initial bytes count
	ByteArray(u32 count)
	{
		bytes.reserve(count);
	}

	void setIndicatorPos(u32 pos)
	{
		pos_i = pos;
	}

	u32 count() const
	{
		return elements.size();
	}

	//! Setters
	// 'n' is a number of the byte array element before which to insert
	// '-1' means inserting at the end
	void setUInt8(u8 elem, s32 n=-1) 		{ setElement(ByteArrayElement(UINT8, 1), &elem, n) }
	void setUInt16(u16 elem, s32 n=-1) 		{ setElement(ByteArrayElement(UINT16, 2), &elem, n) }
	void setUInt32(u32 elem, s32 n=-1);		{ setElement(ByteArrayElement(UINT32, 4), &elem, n) }
	void setUInt64(u64 elem, s32 n=-1);		{ setElement(ByteArrayElement(UINT64, 8), &elem, n) }

	void setChar(c8 elem, s32 n=-1) 		{ setElement(ByteArrayElement(CHAR, 1), &elem, n) }
	void setShort(s16 elem, s32 n=-1) 		{ setElement(ByteArrayElement(SHORT, 2), &elem, n) }
	void setInt(s32 elem, s32 n=-1); 		{ setElement(ByteArrayElement(INT, 4), &elem, n) }
	void setLongInt(s64 elem, s32 n=-1);	{ setElement(ByteArrayElement(LONG_INT, 8), &elem, n) }

	void setFloat(f32 elem, s32 n=-1); 		{ setElement(ByteArrayElement(FLOAT, 4), &elem, n) }
	void setDouble(f64 elem, s32 n=-1); 	{ setElement(ByteArrayElement(DOUBLE, 8), &elem, n) }

	//! Getters
	// 'n' is a number of the byte array element
	u8 getUInt8(u32 n) const
	{
		return getElement(n)[0];
	}
	u16 getUInt16(u32 n) const
	{
		auto elem = getElement(n);

		u16 elem_u16 = elem[1];
		elem_u16 |= ((u16)elem[0]) << 8;

		return elem_u16;
	}
	u32 getUInt32(u32 n) const
	{
		auto elem = getElement(n);

		u32 elem_u32 = elem[3];
		elem_u32 |= ((u32)elem[2]) << 8;
		elem_u32 |= ((u32)elem[1]) << 16;
		elem_u32 |= ((u32)elem[0]) << 24;

		return elem_u32;
	}
	u64 getUInt64(u32 n) const
	{
		auto elem = getElement(n);

		u64 elem_u64 = elem[7];
		elem_u64 |= ((u64)elem[6]) << 8;
		elem_u64 |= ((u64)elem[5]) << 16;
		elem_u64 |= ((u64)elem[4]) << 24;
		elem_u64 |= ((u64)elem[3]) << 32;
		elem_u64 |= ((u64)elem[4]) << 40;
		elem_u64 |= ((u64)elem[4]) << 48;
		elem_u64 |= ((u64)elem[4]) << 56;

		return elem_u64;
	}

	s8 getChar(u32 n) const
	{
		return (s8)getUInt8(n);
	}
	s16 getShort(u32 n) const
	{
		return (s16)getUInt16(n);
	}
	s32 getInt(u32 n) const
	{
		return (s32)getUInt32(n);
	}
	s64 getLongInt(u32 n) const
	{
		return (s64)getUInt64(n);
	}
	f32 getFloat(u32 n) const
	{
		return (f32)getUInt32(n);
	}
	f64 getDouble(u32 n) const
	{
		return (f64)getUInt64(n);
	}
private:
	u32 countBytesBefore(u32 n) const;

	std::vector<u8> getElement(u32 n) const;
	void setElement(const ByteArrayElement &elem, void *data, s32 n=-1);
};


u32 ByteArray::countBytesBefore(u32 n)
{
	if (n >= count()) {
		SDL_LogWarn(LC_ASS, "ByteArray::countBytesBefore() Access to the element outside of the byte array");
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
		SDL_LogWarn(LC_ASS, "ByteArray::getElement() Access to the element outside of the byte array");
		return {};
	}

	std::vector<u8> elem_bytes;

	auto &elem = elements[n];
	for (u32 i = 0; i < elem.bytes_count; i++)
		elem_bytes.push_back(bytes[elem.offset+i]);

	return elem_bytes;
}

ByteArray::setElement(const ByteArrayElement &elem, void *data, s32 n)
{
	std::vector<u8> elem_bytes;

	switch (elem.type) {
		case UINT8:
		case CHAR: {
			elem_bytes.push_back(*(u8*)(data));
			break;
		}
		case UINT16:
		case SHORT: {
			u16 elem_u16 = *(u16*)(data);
			elem_bytes.push_back((u8)elem_u16 >> 8);
			elem_bytes.push_back((u8)elem_u16 && 0xFF);
			break;
		}
		case UINT32:
		case INT:
		case FLOAT: {
			u16 elem_u32 = *(u32*)(data);
			elem_bytes.push_back((u8)elem_u32 >> 24);
			elem_bytes.push_back((u8)elem_u32 >> 16 && 0xFF);
			elem_bytes.push_back((u8)elem_u32 >> 8 && 0xFF);
			elem_bytes.push_back((u8)elem_u32 && 0xFF);
			break;
		}
		case UINT64:
		case LONG_INT:
		case DOUBLE: {
			u16 elem_u64 = *(u64*)(data);
			elem_bytes.push_back((u8)elem_u64 >> 56);
			elem_bytes.push_back((u8)elem_u64 >> 48 && 0xFF);
			elem_bytes.push_back((u8)elem_u64 >> 40 && 0xFF);
			elem_bytes.push_back((u8)elem_u64 >> 32 && 0xFF);
			elem_bytes.push_back((u8)elem_u64 >> 24 && 0xFF);
			elem_bytes.push_back((u8)elem_u64 >> 16 && 0xFF);
			elem_bytes.push_back((u8)elem_u64 >> 8 && 0xFF);
			elem_bytes.push_back((u8)elem_u64 && 0xFF);
			break;
		}
	}

	if (n < count()) {
		auto &n_elem = elements.at(n);

		if (n_elem.type != elem.type || n_elem.bytes_count != elem.bytes_count) {
			SDL_LogWarn("ByteArray::setElement() Can not replace the element to the another one with differing bytes count and type");
			return;
		}
	}
	// If n == -1, add the bytes at the end, else replace to them
	u32 offset = countBytesBefore(n)+1;
	for (u32 i = 0; i < elem_bytes.size(); i++)
		if (n < count())
			bytes[offset + i] = elem_bytes[i];
		else
			bytes.push_back(elem_bytes[i]);

	if (n >= count()) {
		elem.offset = bytes.size() - elem_bytes.size();
		elements.push_back(std::move(elem));
	}
}
