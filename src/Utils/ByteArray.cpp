#include "ByteArray.h"

namespace utils
{

u32 ByteArray::countBytesBefore(u32 n)
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
			elem_bytes.push_back((u8)elem_u16 & 0xFF);
			break;
		}
		case UINT32:
		case INT:
		case FLOAT: {
			u16 elem_u32 = *(u32*)(data);
			elem_bytes.push_back((u8)elem_u32 >> 24);
			elem_bytes.push_back((u8)elem_u32 >> 16 & 0xFF);
			elem_bytes.push_back((u8)elem_u32 >> 8 & 0xFF);
			elem_bytes.push_back((u8)elem_u32 & 0xFF);
			break;
		}
		case UINT64:
		case LONG_INT:
		case DOUBLE: {
			u16 elem_u64 = *(u64*)(data);
			elem_bytes.push_back((u8)elem_u64 >> 56);
			elem_bytes.push_back((u8)elem_u64 >> 48 & 0xFF);
			elem_bytes.push_back((u8)elem_u64 >> 40 & 0xFF);
			elem_bytes.push_back((u8)elem_u64 >> 32 & 0xFF);
			elem_bytes.push_back((u8)elem_u64 >> 24 & 0xFF);
			elem_bytes.push_back((u8)elem_u64 >> 16 & 0xFF);
			elem_bytes.push_back((u8)elem_u64 >> 8 & 0xFF);
			elem_bytes.push_back((u8)elem_u64 & 0xFF);
			break;
		}
	}

	bool replace = n < count() && n != -1;

	if (replace) {
		auto &n_elem = elements.at(n);

		if (n_elem.type != elem.type || n_elem.bytes_count != elem.bytes_count) {
			SDL_LogWarn("ByteArray::setElement() Can not replace the element to the another one with differing bytes count and type");
			return;
		}
	}
	// If n == -1, add the bytes at the end, else replace to them
	u32 offset = countBytesBefore(n)+1;
	for (u32 i = 0; i < elem_bytes.size(); i++)
		if (replace)
			bytes[offset + i] = elem_bytes[i];
		else
			bytes.push_back(elem_bytes[i]);

	if (!replace) {
		elem.offset = bytes.size() - elem_bytes.size();
		elements.push_back(std::move(elem));
	}
}

}
