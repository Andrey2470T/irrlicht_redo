#pragma once

#include "ExtBasicIncludes.h"
#include "Utils/Matrix4.h"

namespace img
{
    class color8;
    class colorf;
}

namespace utils
{

enum ByteArrayElementType : u8
{
    U8,
    U16,
    U32,
    FLOAT,
    V2F,
    V3F,
    MAT4,
    COLOR_RGB8,
    COLOR_RGBA8,
    COLORF
};

size_t sizeOfElement(const ByteArrayElementType &elemType);

// Can be basic type (u8, u16, f32) and complex (v2f, v3f and mat4)
struct ByteArrayElement
{
    std::string Name;
    ByteArrayElementType Type;
    u32 BytesCount = 0;
    u32 BytesOffset = 0; // in bytes from the begin

    bool operator==(const ByteArrayElement other) const
    {
        return (Name == other.Name && Type == other.Type &&
            BytesCount == other.BytesCount && BytesOffset == other.BytesOffset);
    }
};

size_t sizeOfDescriptor(const std::vector<ByteArrayElement> &desc);

struct ByteArrayDescriptor
{
    std::string Name;
    std::vector<ByteArrayElement> Elements;

    ByteArrayDescriptor(const std::string &_Name, const std::vector<ByteArrayElement> &_Elements);
    ByteArrayDescriptor(const ByteArrayDescriptor &other);

    bool operator==(const ByteArrayDescriptor &other) const
    {
        return (Name == other.Name && Elements == other.Elements);
    }
};

//! Array of a bytes sequence formatted by some descriptor
class ByteArray
{
    //! Elements descriptor
    ByteArrayDescriptor Descriptor;
    //! Size of all descriptor elements
    size_t DescriptorSize;
    //! Count of 'std::vector<ByteArrayElement>' vectors
    u32 ElementsSetsCount = 0;
    //! Current index of 'std::vector<ByteArrayElement>' vectors
    u32 ElementsSetIndex = 0;
	//! Bytes storage
    std::vector<u8> Bytes;
public:
    //! Constructor. 'count' is an initial bytes count
    ByteArray(u32 bytesCount);
    //! Constructor. '_ElementsSetsCount' is an initial 'std::vector<ByteArrayElement>' count
    ByteArray(const ByteArrayDescriptor &desc, u32 _ElementsSetsCount);

    ByteArray(const ByteArray &other);

    //! Count of 'std::vector<ByteArrayElement>' vectors
	u32 count() const
	{
        return ElementsSetsCount;
	}

    //! Size of 'Bytes' vector
    u32 bytesCount() const
    {
        return Bytes.size();
    }

    void reallocate(u32 _ElementsSetsCount, const u8 *addData=nullptr);

    void clear()
    {
        ElementsSetsCount = 0;
        Bytes.clear();
        ElementsSetIndex = 0;
    }

    u32 getElemsSetIndex() const
    {
        return ElementsSetIndex;
    }

    void setElemsSetIndex(u32 index)
    {
        ElementsSetIndex = index;
    }

    const void *data() const
	{
        return reinterpret_cast<const void*>(Bytes.data());
	}

	void *data()
	{
        return reinterpret_cast<void*>(Bytes.data());
	}

    //! 'elemsSetN' is a number of 'std::vector<ByteArrayElement>' vector
    //! 'elemN' is a number of the element inside the ByteArrayDescriptor

	//! Setters
    void setUInt8(u8 elem, u32 elemN, s64 elemsSetN=-1);
    void setUInt16(u16 elem, u32 elemN, s64 elemsSetN=-1);
    void setUInt32(u32 elem, u32 elemN, s64 elemsSetN=-1);
    void setFloat(f32 elem, u32 elemN, s64 elemsSetN=-1);
    void setV2F(v2f elem, u32 elemN, s64 elemsSetN=-1);
    void setV3F(v3f elem, u32 elemN, s64 elemsSetN=-1);
    void setM4x4(const matrix4 &elem, u32 elemN, s64 elemsSetN=-1);
    void setColor8(const img::color8 &elem, u32 elemN, s64 elemsSetN=-1);
    void setColorf(const img::colorf &elem, u32 elemN, s64 elemsSetN=-1);


	//! Getters
    u8 getUInt8(u32 elemsSetN, u32 elemN) const;
    u16 getUInt16(u32 elemsSetN, u32 elemN) const;
    u32 getUInt32(u32 elemsSetN, u32 elemN) const;
    f32 getFloat(u32 elemsSetN, u32 elemN) const;
    v2f getV2F(u32 elemsSetN, u32 elemN) const;
    v3f getV3F(u32 elemsSetN, u32 elemN) const;
    matrix4 getM4x4(u32 elemsSetN, u32 elemN) const;
    img::color8 getColorRGB8(u32 elemsSetN, u32 elemN) const;
    img::color8 getColorRGBA8(u32 elemsSetN, u32 elemN) const;
    img::colorf getColorf(u32 elemsSetN, u32 elemN) const;
private:
    void getElement(void *data, u32 elemN, u32 elemsSetN) const;
    void setElement(const void *data, u32 elemN, s64 elemsSetN=-1);
};

}
