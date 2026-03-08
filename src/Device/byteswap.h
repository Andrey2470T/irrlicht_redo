#include <SDL_endian.h>

#define bswap_16(X) SDL_Swap16(X)
#define bswap_32(X) SDL_Swap32(X)
#define bswap_64(X) SDL_Swap64(X)
