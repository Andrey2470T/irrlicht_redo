#include "ByteArray.h"

namespace utils
{

u32 ByteArray::bytesCount() const
{
	return countBytesBefore(count());
}

u32 ByteArray::countBytesBefore(u32 n) const
{
	if (n > count()) {
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

	elem_bytes.resize(elem.bytes_count);
	for (u32 i = 0; i < elem.bytes_count; i++)
		elem_bytes[i] = bytes[elem.offset+i];

	return elem_bytes;
}

void ByteArray::setElement(ByteArrayElement &&elem, void *data, std::optional<u32> n)
{
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

	u32 offset;

	if (!n) {
		offset = bytesCount();
		bytes.resize(offset + elem_bytes.size());
	}
	else if (n.value() < count()) {
		auto &n_elem = elements.at(n.value());

		if (n_elem.type != elem.type || n_elem.bytes_count != elem.bytes_count) {
			SDL_LogWarn(LC_APP, "ByteArray::setElement() Can not replace the\
				element to the another one with differing bytes count and type");
			return;
		}
		offset = countBytesBefore(n.value());
	}

	for (u32 i = 0; i < elem_bytes.size(); i++)
		bytes[offset + 1 + i] = elem_bytes[i];

	if (!n) {
		elem.offset = offset;
		elements.push_back(std::move(elem));
	}
}

}
