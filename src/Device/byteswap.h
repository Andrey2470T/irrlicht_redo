#include "irrTypes.h"

namespace os
{
class Byteswap
{
public:
	static u16 byteswap(u16 num);
	static s16 byteswap(s16 num);
	static u32 byteswap(u32 num);
	static s32 byteswap(s32 num);
	static u64 byteswap(u64 num);
	static s64 byteswap(s64 num);
	static f32 byteswap(f32 num);
	// prevent accidental swapping of chars
	static inline u8 byteswap(u8 num) { return num; }
	static inline c8 byteswap(c8 num) { return num; }
};
}
