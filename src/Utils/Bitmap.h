#include "ExtBasicIncludes.h"

namespace utils
{

class Bitmap
{
	std::vector<u8> data;
	u32 width, height;
public:
    Bitmap(u32 _width, u32 _height)
        : data(getBytePos(_width * _height)+1), width(_width), height(_height)
    {}
    
    u32 size() const // in bits
    {
    	return width * height;
    }

    u32 bytesSize() const // in bytes
    {
        return data.size();
    }
    
    void set(u32 x, u32 y, bool value=true); // sets a bit
    bool get(u32 x, u32 y) const; // gets a bit

    void setByte(u32 n, u8 value=true); // sets a byte
    u8 getByte(u32 n); // gets a byte
private:
    u32 getBytePos(u32 bitNum) const
    {
    	return bitNum >> 3; // equivalent to "bitNum / 8"
    }
    
    u32 getRelBitPos(u32 bitNum) const
    {
    	return bitNum & 7; // equivalent to "bitNum % 8"
    }
};

}
