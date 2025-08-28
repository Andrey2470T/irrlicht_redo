#include "ByteArray.h"

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

    memcpy(bytes.data(), (u8 *)add_bytes->data()+offset, count);
}

u32 ByteArray::countBytesBefore(u32 n) const
{
	if (n > count()) {
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

	//InfoStream << "ByteArray::getElement() type=" << (u8)elem.type << ", bytes_count=" << elem.bytes_count << ", offset=" << elem.offset << "\n";
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
			elem_bytes[0] = (u8)(elem_u16 >> 8);
			elem_bytes[1] = (u8)(elem_u16 & 0xFF);
			break;
		}
		case BasicType::UINT32:
		case BasicType::INT:
		case BasicType::FLOAT: {
			elem_bytes.resize(4);
			u32 elem_u32 = *(u32*)(data);
			elem_bytes[0] = (u8)(elem_u32 >> 24);
			elem_bytes[1] = (u8)(elem_u32 >> 16 & 0xFF);
			elem_bytes[2] = (u8)(elem_u32 >> 8 & 0xFF);
			elem_bytes[3] = (u8)(elem_u32 & 0xFF);
			break;
		}
		case BasicType::UINT64:
		case BasicType::LONG_INT:
		case BasicType::DOUBLE: {
			elem_bytes.resize(8);
			u64 elem_u64 = *(u64*)(data);
			elem_bytes[0] = (u8)(elem_u64 >> 56);
			elem_bytes[1] = (u8)(elem_u64 >> 48 & 0xFF);
			elem_bytes[2] = (u8)(elem_u64 >> 40 & 0xFF);
			elem_bytes[3] = (u8)(elem_u64 >> 32 & 0xFF);
			elem_bytes[4] = (u8)(elem_u64 >> 24 & 0xFF);
			elem_bytes[5] = (u8)(elem_u64 >> 16 & 0xFF);
			elem_bytes[6] = (u8)(elem_u64 >> 8 & 0xFF);
			elem_bytes[7] = (u8)(elem_u64 & 0xFF);
			break;
		}
		default:
			break;
	}

    auto &n_elem = elements.at(n);
    n_elem.type = elem.type;
    n_elem.bytes_count = elem.bytes_count;
    n_elem.offset = countBytesBefore(n);

	//InfoStream << "ByteArray::setElement() type=" << (u8)n_elem.type << ", bytes_count=" << n_elem.bytes_count << ", offset=" << n_elem.offset << "\n";

	for (u32 i = 0; i < elem_bytes.size(); i++) {
		//InfoStream << "ByteArray::setElement(): byte i =" << elem_bytes[i] << "\n";
        bytes[n_elem.offset + i] = elem_bytes[i];
	}
}

}
