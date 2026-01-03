#include "ByteArray.h"
#include "Image/Color.h"

namespace utils
{

size_t sizeOfElement(const ByteArrayElementType &elemType)
{
    switch(elemType) {
    case ByteArrayElementType::U8:
        return sizeof(u8);
    case ByteArrayElementType::U16:
        return sizeof(u16);
    case ByteArrayElementType::U32:
        return sizeof(u32);
    case ByteArrayElementType::FLOAT:
        return sizeof(f32);
    case ByteArrayElementType::V2F:
        return sizeof(v2f);
    case ByteArrayElementType::V3F:
        return sizeof(v3f);
    case ByteArrayElementType::MAT4:
        return sizeof(matrix4);
    case ByteArrayElementType::COLOR_RGB8:
        return sizeof(u8) * 3;
    case ByteArrayElementType::COLOR_RGBA8:
        return sizeof(u8) * 4;
    case ByteArrayElementType::COLORF:
        return sizeof(f32) * 4;
    default:
        return 0;
    }
}

size_t sizeOfDescriptor(const std::vector<ByteArrayElement> &desc)
{
    size_t size = 0;

    for (const auto &elem : desc)
        size += sizeOfElement(elem.Type);

    return size;
}

ByteArrayDescriptor::ByteArrayDescriptor(const std::string &_Name, const std::vector<ByteArrayElement> &_Elements)
    : Name(_Name), Elements(std::move(_Elements))
{
    u32 offset = 0;

    for (auto &elem : Elements) {
        elem.BytesOffset = offset;
        elem.BytesCount = sizeOfElement(elem.Type);
        offset += elem.BytesCount;
    }
}

ByteArrayDescriptor::ByteArrayDescriptor(const ByteArrayDescriptor &other)
    : Name(other.Name), Elements(other.Elements)
{}

ByteArray::ByteArray(u32 bytesCount)
    : Descriptor("", {{"", ByteArrayElementType::U8}})
{
    DescriptorSize = sizeOfDescriptor(Descriptor.Elements);
    reallocate(bytesCount);
}

ByteArray::ByteArray(const ByteArrayDescriptor &desc, u32 _ElementsSetsCount)
    : Descriptor(desc)
{
    DescriptorSize = sizeOfDescriptor(Descriptor.Elements);
    reallocate(_ElementsSetsCount);
}

ByteArray::ByteArray(const ByteArray &other)
    : Descriptor(other.Descriptor), DescriptorSize(other.DescriptorSize),
      ElementsSetsCount(other.ElementsSetsCount), Bytes(other.Bytes)
{}

void ByteArray::reallocate(u32 _ElementsSetsCount, const u8 *addData)
{
    if (_ElementsSetsCount == ElementsSetsCount)
        return;

    u32 oldElementsSetsCount = ElementsSetsCount;
    ElementsSetsCount = _ElementsSetsCount;
    Bytes.resize(DescriptorSize * ElementsSetsCount);

    if (addData) {
        memcpy(Bytes.data() + DescriptorSize * oldElementsSetsCount,
            addData, (ElementsSetsCount-oldElementsSetsCount) * DescriptorSize);
    }
}

void ByteArray::setUInt8(u8 elem, u32 elemN, s64 elemsSetN)                     { setElement(&elem, elemN, elemsSetN); }
void ByteArray::setUInt16(u16 elem, u32 elemN, s64 elemsSetN)                   { setElement(&elem, elemN, elemsSetN); }
void ByteArray::setUInt32(u32 elem, u32 elemN, s64 elemsSetN)                   { setElement(&elem, elemN, elemsSetN); }
void ByteArray::setFloat(f32 elem, u32 elemN, s64 elemsSetN)                    { setElement(&elem, elemN, elemsSetN); }
void ByteArray::setV2F(v2f elem, u32 elemN, s64 elemsSetN)                      { setElement(&elem, elemN, elemsSetN); }
void ByteArray::setV3F(v3f elem, u32 elemN, s64 elemsSetN)                      { setElement(&elem, elemN, elemsSetN); }
void ByteArray::setM4x4(const matrix4 &elem, u32 elemN, s64 elemsSetN)          { setElement(&elem, elemN, elemsSetN); }
void ByteArray::setColor8(const img::color8 &elem, u32 elemN, s64 elemsSetN)    {
    std::vector<u8> color = {elem.R(), elem.G(), elem.B(), elem.A()};
    setElement(color.data(), elemN, elemsSetN);
}
void ByteArray::setColorf(const img::colorf &elem, u32 elemN, s64 elemsSetN)    { setElement(&elem, elemN, elemsSetN); }

//! Getters
// 'n' is a number of the byte array element
u8 ByteArray::getUInt8(u32 elemsSetN, u32 elemN) const
{
    u8 elem;
    getElement(&elem, elemsSetN, elemN);
    return elem;
}
u16 ByteArray::getUInt16(u32 elemsSetN, u32 elemN) const
{
    u16 elem;
    getElement(&elem, elemsSetN, elemN);
    return elem;
}
u32 ByteArray::getUInt32(u32 elemsSetN, u32 elemN) const
{
    u32 elem;
    getElement(&elem, elemsSetN, elemN);
    return elem;
}
f32 ByteArray::getFloat(u32 elemsSetN, u32 elemN) const
{
    f32 elem;
    getElement(&elem, elemsSetN, elemN);
    return elem;
}
v2f ByteArray::getV2F(u32 elemsSetN, u32 elemN) const
{
    v2f elem;
    getElement(&elem, elemsSetN, elemN);
    return elem;
}
v3f ByteArray::getV3F(u32 elemsSetN, u32 elemN) const
{
    v3f elem;
    getElement(&elem, elemsSetN, elemN);
    return elem;
}
matrix4 ByteArray::getM4x4(u32 elemsSetN, u32 elemN) const
{
    matrix4 elem;
    getElement(&elem, elemsSetN, elemN);
    return elem;
}
img::color8 ByteArray::getColorRGB8(u32 elemsSetN, u32 elemN) const
{
    img::color8 elem(img::PF_RGB8);
    std::vector<u8> color = {elem.R(), elem.G(), elem.B(), elem.A()};
    getElement(color.data(), elemsSetN, elemN);
    elem.set(color[0], color[1], color[2], color[3]);
    return elem;
}
img::color8 ByteArray::getColorRGBA8(u32 elemsSetN, u32 elemN) const
{
    img::color8 elem(img::PF_RGBA8);
    std::vector<u8> color = {elem.R(), elem.G(), elem.B(), elem.A()};
    getElement(color.data(), elemsSetN, elemN);
    elem.set(color[0], color[1], color[2], color[3]);
    return elem;
}
img::colorf ByteArray::getColorf(u32 elemsSetN, u32 elemN) const
{
    img::colorf elem;
    getElement(&elem, elemsSetN, elemN);
    return elem;
}

void ByteArray::getElement(void *data, u32 elemN, s64 elemsSetN) const
{
    assert(elemsSetN < ElementsSetsCount);
    auto &elem = Descriptor.Elements.at(elemN);
    memcpy(data, Bytes.data() + DescriptorSize * elemsSetN + elem.BytesOffset, elem.BytesCount);
}

void ByteArray::setElement(const void *data, u32 elemN, s64 elemsSetN)
{
    auto &elem = Descriptor.Elements.at(elemN);

    if (elemsSetN != -1) {
        assert(elemsSetN < ElementsSetsCount);
        ByteIndex = DescriptorSize * elemsSetN + elem.BytesOffset;
    }
    memcpy(Bytes.data() + ByteIndex, data, elem.BytesCount);
    ByteIndex += elem.BytesCount;
}

}
