#include "TTFont.h"
#include "Core/TimeCounter.h"
#include "Image/ImageLoader.h"
#include "Utils/String.h"
#include "Image/Converting.h"
#include <iostream>
#include <Utils/MathFuncs.h>

namespace render
{

TTFont::TTFont(TTF_Font *_font, FontMode _mode, u32 _size, bool _transparent, u32 _shadow_offset, u32 _shadow_alpha)
    : font(_font), mode(_mode), style((FontStyle)TTF_GetFontStyle(_font)), curSize(_size),
      hasTransparency(_transparent), shadowOffset(_shadow_offset), shadowAlpha(_shadow_alpha)
{
    // Cache all glyphs provided by the font file
    for (wchar_t ch = 0; ch < MAX_GLYPHS_COUNT; ch++) {
        if (hasGlyph(ch))
            glyphsSet.push_back(ch);
    }
}

TTFont::~TTFont()
{
    TTF_CloseFont(font);
}

bool TTFont::init()
{
    if (TTF_Init() != 0) {
        ErrorStream << "TTFont::Init() failed to init SDL2_ttf: " << SDL_GetError() << "\n";
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

u32 TTFont::getGlyphsNum() const
{
    return glyphsSet.size();
}

const std::vector<wchar_t> &TTFont::getGlyphsSet() const
{
    return glyphsSet;
}

u32 TTFont::getTextWidth(const std::wstring &text) const
{
    s32 w = 0;
    s32 h = 0;

    auto str16 = wstring_to_uint16(text);

    TTF_SizeUNICODE(font, str16.data(), &w, &h);

    return (u32)w;
}

u32 TTFont::getTextHeight(const std::wstring &text) const
{
    s32 w = 0;
    s32 h = 0;

    auto str16 = wstring_to_uint16(text);

    TTF_SizeUNICODE(font, str16.data(), &w, &h);

    return (u32)h;
}

v2u TTFont::getTextSize(const std::wstring &text) const
{
    return v2u(getTextWidth(text), getTextHeight(text));
}

u32 TTFont::getLineHeight() const
{
    return getTextHeight(L"S");
}

u32 TTFont::getFontHeight() const
{
    return TTF_FontHeight(font);
}

u32 TTFont::getKerningSizeForTwoChars(wchar_t ch1, wchar_t ch2) const
{
    Uint16 glyph1 = static_cast<Uint16>(ch1);
    Uint16 glyph2 = static_cast<Uint16>(ch2);

    return TTF_GetFontKerningSizeGlyphs32(font, glyph1, glyph2);
}

s32 TTFont::getFontAscent() const
{
    return TTF_FontAscent(font);
}

s32 TTFont::getFontDescent() const
{
    return TTF_FontDescent(font);
}

void TTFont::getGlyphMetrics(wchar_t ch, s32 *offsetx, s32 *offsety, s32 *advance) const
{
    if (!offsetx || !offsety || !advance) {
        ErrorStream << "TTFont::getGlyphMetrics() no offset or/and advance provided\n";
        return;
    }

    Uint16 glyph = static_cast<Uint16>(ch);
    s32 minx, maxx, miny, maxy;
    TTF_GlyphMetrics(font, glyph, &minx, &maxx, &miny, &maxy, advance);
    *offsetx = minx;
    *offsety = miny;
}

u32 TTFont::getCurrentSize() const
{
    return curSize;
}

#define POINTS_PER_INCH 72.0

u32 TTFont::getCurrentPixelSize(u32 dpi) const
{
    return (u32)round(curSize * (f32)dpi / POINTS_PER_INCH);
}

bool TTFont::isTransparent() const
{
    return hasTransparency;
}

std::optional<u32> TTFont::getCharFromPos(const std::wstring &str, s32 pixel_x) const
{
    s32 x = 0;
    u32 num = 0;
    wchar_t prevChar = 0;
    for (const wchar_t &c : str) {
        x += getTextWidth(std::to_wstring(c));
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
    Uint16 glyph = static_cast<Uint16>(ch);
    return TTF_GlyphIsProvided(font, glyph);
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
    Uint16 glyph = static_cast<Uint16>(ch);
    //core::InfoStream << "getGlyphImage: create glyph for " << ch << ", time: " << TimeCounter::getRealTime() << "\n";
    SDL_Surface *surf = TTF_RenderGlyph_Blended(font, glyph, fg_color);
    //core::InfoStream << "getGlyphImage: created, converting to img::Image, time: " << TimeCounter::getRealTime() << "\n";

    if (!surf) {
        ErrorStream << "TTFont::getGlyphImage() failed to render glyph for " << ch << ": " << TTF_GetError() << "\n";
        return nullptr;
    }

    //InfoStream << "getGlyphImage:1\n";
    img::Image *surf_img = img::convertSDLSurfaceToImage(surf, false);
    //core::InfoStream << "getGlyphImage: created, converted, time: " << TimeCounter::getRealTime() << "\n";
    //InfoStream << "getGlyphImage:2\n";

    SDL_FreeSurface(surf);
   //InfoStream << "getGlyphImage:3\n";

    if (ch == L'a') {
        img::ImageLoader::save(surf_img, std::string("/home/andrey/minetests/luanti_fork/cache/atlases/") + std::to_string(hash(this)) + "glyph_a.png");
    }

    return surf_img;
}

img::Image *TTFont::drawText(const std::wstring &text, const img::color8 &color)
{
    SDL_Color fg_color = {
        color.R(), color.G(), color.B(), color.A()
    };
    auto str16 = wstring_to_uint16(text);

    SDL_Surface *surf = TTF_RenderUNICODE_Solid(
        font, str16.data(), fg_color);

    img::Image *surf_img = img::convertSDLSurfaceToImage(surf, false);

    SDL_FreeSurface(surf);

    return surf_img;
}

u64 TTFont::hash(const TTFont *font)
{
    return (((u64)font->getCurrentSize() << 6) |//size: 32 bits
            ((u64)font->isTransparent() << 5) | // transparent: 1 bit
           ((u64)font->getStyle() << 2) |       // style: 3 bits
            (u64)font->getMode());              // mode: 2 bits
}

u64 TTFont::hash(u32 size, bool transparent, FontStyle style, FontMode mode)
{
    return (
        ((u64)size << 6) |
        ((u64)transparent << 5) |
        ((u64)style << 2) |
        (u64)mode);
}

bool TTFont::operator==(const TTFont *other) const
{
    return hash(this) == hash(other);
}

}
