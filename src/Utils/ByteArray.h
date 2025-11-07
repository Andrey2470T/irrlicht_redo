#pragma once

#include "ExtBasicIncludes.h"
#include "Utils/Matrix4.h"

namespace utils
{

class ByteArray
{
	struct ByteArrayElement
	{
		BasicType type;
		u32 bytes_count;
        u32 offset = 0; // in bytes from the begin
	};

	//! Bytes storage
	std::vector<u8> bytes;
	//! Elements (base types) saving inside 'bytes'
	std::vector<ByteArrayElement> elements;
public:
	ByteArray() = default;

    //! Constructor. 'count' is an initial bytes count
    ByteArray(u32 elemCount, u32 bytesCount)
	{
        reallocate(elemCount, bytesCount);
	}

	u32 count() const
	{
		return elements.size();
	}

    u32 bytesCount() const
    {
        return bytes.size();
    }

    void reallocate(u32 elemCount, u32 bytesCount);

    void extendBytes(const ByteArray *add_bytes, u32 offset, u32 count);

    void clear()
    {
        elements.clear();
        bytes.clear();
    }

    const void *data() const
	{
        const u8 *bytes_data = bytes.data();
        return reinterpret_cast<const void*>(bytes_data);
	}

	void *data()
	{
        u8 *bytes_data = bytes.data();
        return reinterpret_cast<void*>(bytes_data);
	}

	//! Setters
    // 'n' is a number of the byte array element which is necessary to change
    void setUInt8(u8 elem, u32 n);
    void setUInt16(u16 elem, u32 n);
    void setUInt32(u32 elem, u32 n);
    void setUInt64(u64 elem, u32 n);

    void setChar(c8 elem, u32 n);
    void setShort(s16 elem, u32 n);
    void setInt(s32 elem, u32 n);
    void setLongInt(s64 elem, u32 n);

    void setFloat(f32 elem, u32 n);
    void setDouble(f64 elem, u32 n);

    void setV2U(v2u elem, u32 n);
    void setV2I(v2i elem, u32 n);
    void setV2F(v2f elem, u32 n);

    void setV3U(v3u elem, u32 n);
    void setV3I(v3i elem, u32 n);
    void setV3F(v3f elem, u32 n);

    void setM4x4(const matrix4 &elem, u32 n);

	//! Getters
	// 'n' is a number of the byte array element
    u8 getUInt8(u32 n) const;
    u16 getUInt16(u32 n) const;
    u32 getUInt32(u32 n) const;
    u64 getUInt64(u32 n) const;

    v2u getV2U(u32 n) const;
    v2i getV2I(u32 n) const;
    v2f getV2F(u32 n) const;

    v3u getV3U(u32 n) const;
    v3i getV3I(u32 n) const;
    v3f getV3F(u32 n) const;

    matrix4 getM4x4(u32 n) const;

    s8 getChar(u32 n) const;
    s16 getShort(u32 n) const;
    s32 getInt(u32 n) const;
    s64 getLongInt(u32 n) const;
    f32 getFloat(u32 n) const;
    f64 getDouble(u32 n) const;
private:
	u32 countBytesBefore(u32 n) const;

	std::vector<u8> getElement(u32 n) const;
    void setElement(ByteArrayElement &&elem, void *data, u32 n);
};

}
