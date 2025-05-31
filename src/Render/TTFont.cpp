#include "TTFont.h"
#include "Utils/String.h"
#include "Image/Converting.h"

namespace render
{

bool TTFont::isInit = false;

TTFont::TTFont(TTF_Font *_font, FontMode _mode, u32 _size, bool _transparent, u32 _shadow_offset, u32 _shadow_alpha)
    : font(_font), mode(_mode), style((FontStyle)TTF_GetFontStyle(_font)), curSize(_size),
      hasTransparency(_transparent), shadowOffset(_shadow_offset), shadowAlpha(_shadow_alpha)
{
    if (!isInit) {
        ErrorStream << "TTFont: could not create the TrueType font as SDL2_ttf is not initialized\n";
        delete this;
        return;
    }
}

TTFont::~TTFont()
{
    TTF_CloseFont(font);
}

bool TTFont::init()
{
    if (TTF_Init() != 0) {
        ErrorStream << "TTFont::Init() failed to init SDL2_ttf: " << TTF_GetError() << "\n";
        return false;
    }

    return true;
}

void TTFont::free()
{
    TTF_Quit();
}

TTFont *TTFont::load(const std::string &path, u32 size, u32 face, bool antialias,
    bool transparent, u32 shadow_offset, u32 shadow_alpha)
{
    TTF_Font *new_font = TTF_OpenFontIndex(path.c_str(), size, face);

    if (!new_font) {
        ErrorStream << "TTFont::load() could not load the TrueType font: " << TTF_GetError() << "\n";
        return nullptr;
    }

    return new TTFont(new_font, (FontMode)antialias, size, transparent, shadow_offset, shadow_alpha);
}

TTFont *TTFont::loadFromMem(void *mem, u32 size, u32 face, bool antialias,
    bool transparent, u32 shadow_offset, u32 shadow_alpha)
{
    SDL_RWops *rw = SDL_RWFromMem(mem, size);
    TTF_Font *new_font = TTF_OpenFontIndexRW(rw, 1, size, face);

    if (!new_font) {
        ErrorStream << "TTFont::loadFromMem() could not load the TrueType font: " << TTF_GetError() << "\n";
        return nullptr;
    }

    return new TTFont(new_font, (FontMode)antialias, size, transparent, shadow_offset, shadow_alpha);
}

FontMode TTFont::getMode() const
{
	return mode;
}

FontStyle TTFont::getStyle() const
{
	return style;
}

u32 TTFont::getTextWidth(const std::wstring &text) const
{
    s32 w, h;
    std::u16string str16 = wide_to_utf16(text);

    TTF_SizeUNICODE(font, reinterpret_cast<const Uint16 *>(str16.data()), &w, &h);

    return (u32)w;
}

u32 TTFont::getTextHeight(const std::wstring &text) const
{
    s32 w, h;
    std::u16string str16 = wide_to_utf16(text);

    TTF_SizeUNICODE(font, reinterpret_cast<const Uint16 *>(str16.data()), &w, &h);

    return (u32)h;
}

v2u TTFont::getTextSize(const std::wstring &text) const
{
    return v2u(getTextWidth(text), getTextHeight(text));
}

u32 TTFont::getLineHeight() const
{
    return getTextHeight(L"S") + (u32)TTF_FontLineSkip(font);
}

u32 TTFont::getKerningSizeForTwoChars(wchar_t ch1, wchar_t ch2) const
{
    std::u16string ch1_16 = wide_to_utf16(&ch1);
    std::u16string ch2_16 = wide_to_utf16(&ch2);

    return TTF_GetFontKerningSizeGlyphs32(font,
        *reinterpret_cast<const Uint16 *>(ch1_16.data()),
        *reinterpret_cast<const Uint16 *>(ch2_16.data()));
}

u32 TTFont::getCurrentSize() const
{
    return curSize;
}

std::optional<u32> TTFont::getCharFromPos(const std::wstring &str, s32 pixel_x) const
{
    s32 x = 0;
    u32 num = 0;
    wchar_t prevChar = 0;
    for (const wchar_t &c : str) {
        x += getTextWidth(&c);
        x += getKerningSizeForTwoChars(c, prevChar);

        if (x >= pixel_x)
            return num;

        ++num;
        prevChar = c;
    }

    return std::nullopt;
}

bool TTFont::hasGlyph(wchar_t ch) const
{
	std::u16string ch_16 = wide_to_utf16(&ch);
	return TTF_GlyphIsProvided(font, ch_16[0]);
}

void TTFont::setSize(u32 size)
{
    TTF_SetFontSize(font, (s32)size);
    curSize = size;
}

img::Image *TTFont::getGlyphImage(wchar_t ch, const img::color8 &char_color)
{
	if (!hasGlyph(ch))
	    return nullptr;

    SDL_Color fg_color = {
        char_color.R(), char_color.G(), char_color.B(), char_color.A()
    };
    std::u16string ch_16 = wide_to_utf16(&ch);
    SDL_Surface *surf = TTF_RenderGlyph_Solid(
        font, *reinterpret_cast<Uint16 *>(&ch_16), fg_color);

    img::Image *surf_img = img::convertSDLSurfaceToImage(surf);
    img::Image *img_copy = surf_img->copy();

    delete surf_img;

    return img_copy;
}

u64 TTFont::hash() const
{
    return (curSize << 6) | (hasTransparency << 5) | ((u8)style << 2) | (u8)mode;
}

bool TTFont::operator==(const TTFont *other) const
{
    return hash() == other->hash();
}

}
