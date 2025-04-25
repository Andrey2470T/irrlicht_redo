#include "Bitmap.h"

namespace utils
{
	
void Bitmap::set(u32 x, u32 y, bool value) // sets a bit
{
	if (x > width || y > height) {
		ErrorStream << "Bitmap::set(): x,y are out of range\n";
		return;
	}
	u32 index = y * width + x;
	u32 bytepos = getBytePos(index);
	u32 bitShift = getRelBitPos(index);
	
	if (value)
		data[bytepos] |= 1 << bitShift;
	else
	    data[bytepos] &= ~(1 << bitShift);
}

bool Bitmap::get(u32 x, u32 y) const // gets a bit
{
	if (x > width || y > height) {
		ErrorStream << "Bitmap::get(): x, y are out of range\n";
		return false;
	}
	u32 index = y * width + x;
	u32 bytepos = getBytePos(index);
	u32 bitShift = getRelBitPos(index);
	
	return data[bytepos] & (1 << bitShift);
}

void Bitmap::setByte(u32 n, u8 value) // sets a byte
{
	if (n >= data.size()) {
		ErrorStream << "Bitmap::setByte(): n is out of range\n";
		return;
	}

    data[n] = value;
}

u8 Bitmap::getByte(u32 n) // gets a byte
{
	if (n >= data.size()) {
		ErrorStream << "Bitmap::getByte(): n is out of range\n";
        return false;
	}

	return data[n];
}
	
}
