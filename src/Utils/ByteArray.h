#pragma once

#include "BasicIncludes.h"

namespace utils
{

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
	
    u32 bytesCount() const;
	
    const void *data() const
	{
        const u8 *bytes_data = bytes.data();
        return reinterpret_cast<const void*>(bytes_data);
	}

	//! Setters
	// 'n' is a number of the byte array element which is necessary to replace
	// '-1' means inserting at the end
    void setUInt8(u8 elem, s32 n=-1) 		{ setElement({BasicType::UINT8, 1}, &elem, n); }
    void setUInt16(u16 elem, s32 n=-1) 		{ setElement({BasicType::UINT16, 2}, &elem, n); }
    void setUInt32(u32 elem, s32 n=-1)		{ setElement({BasicType::UINT32, 4}, &elem, n); }
    void setUInt64(u64 elem, s32 n=-1)		{ setElement({BasicType::UINT64, 8}, &elem, n); }

    void setChar(c8 elem, s32 n=-1) 		{ setElement({BasicType::CHAR, 1}, &elem, n); }
    void setShort(s16 elem, s32 n=-1) 		{ setElement({BasicType::SHORT, 2}, &elem, n); }
    void setInt(s32 elem, s32 n=-1) 		{ setElement({BasicType::INT, 4}, &elem, n); }
    void setLongInt(s64 elem, s32 n=-1)     { setElement({BasicType::LONG_INT, 8}, &elem, n); }

    void setFloat(f32 elem, s32 n=-1) 		{ setElement({BasicType::FLOAT, 4}, &elem, n); }
    void setDouble(f64 elem, s32 n=-1)      { setElement({BasicType::DOUBLE, 8}, &elem, n); }

	void setV2U(v2u elem, s32 n=-1)
	{
		setUInt32(elem.X, n);
		setUInt32(elem.Y, n != -1 ? n + 1 : n);
	}

	void setV2I(v2i elem, s32 n=-1)
	{
        setInt(elem.X, n);
        setInt(elem.Y, n != -1 ? n + 1 : n);
	}

	void setV2F(v2f elem, s32 n=-1)
	{
		setFloat(elem.X, n);
		setFloat(elem.Y, n != -1 ? n + 1 : n);
	}

	void setV3I(v3i elem, s32 n=-1)
	{
        setInt(elem.X, n);
        setInt(elem.Y, n != -1 ? n + 1 : n);
        setInt(elem.Z, n != -1 ? n + 2 : n);
	}

	void setV3F(v3f elem, s32 n=-1)
	{
		setFloat(elem.X, n);
		setFloat(elem.Y, n != -1 ? n + 1 : n);
		setFloat(elem.Z, n != -1 ? n + 2 : n);
	}

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

	v2u getV2U(u32 n) const
	{
		return v2u(getUInt32(n), getUInt32(n + 1));
	}

	v2i getV2I(u32 n) const
	{
        return v2i(getInt(n), getInt(n + 1));
	}

	v2f getV2F(u32 n) const
	{
		return v2f(getFloat(n), getFloat(n + 1));
	}

	v3i getV3I(u32 n) const
	{
        return v3i(getInt(n), getInt(n + 1), getInt(n + 2));
	}

	v3f getV3F(u32 n) const
	{
		return v3f(getFloat(n), getFloat(n + 1), getFloat(n + 2));
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
    void setElement(ByteArrayElement &&elem, void *data, s32 n=-1);
};

}
