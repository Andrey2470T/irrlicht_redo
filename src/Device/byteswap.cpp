#include "byteswap.h"
#include "Utils/irrMath.h"

#ifdef _IRR_USE_SDL3_
	#include <SDL3/SDL_endian.h>
#else
	#include <SDL_endian.h>
#endif

#define bswap_16(X) SDL_Swap16(X)
#define bswap_32(X) SDL_Swap32(X)
#define bswap_64(X) SDL_Swap64(X)

namespace os
{
u16 Byteswap::byteswap(u16 num)
{
	return bswap_16(num);
}
s16 Byteswap::byteswap(s16 num)
{
	return bswap_16(num);
}
u32 Byteswap::byteswap(u32 num)
{
	return bswap_32(num);
}
s32 Byteswap::byteswap(s32 num)
{
	return bswap_32(num);
}
u64 Byteswap::byteswap(u64 num)
{
	return bswap_64(num);
}
s64 Byteswap::byteswap(s64 num)
{
	return bswap_64(num);
}
f32 Byteswap::byteswap(f32 num)
{
	u32 tmp = IR(num);
	tmp = bswap_32(tmp);
	return (FR(tmp));
}
}
