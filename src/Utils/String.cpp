#include "String.h"
#include <codecvt>
#include <locale>

namespace utils
{

    std::wstring utf8_to_wide(const std::string &str)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.from_bytes(str);
    }
    
    std::string wide_to_utf8(const std::wstring &str)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(str);
    }
    
    std::u32string wide_to_utf32(const std::wstring &str)
    {
        if (sizeof(wchar_t) == 4)
            return std::u32string(reinterpret_cast<const char32_t *>(str.data()));
    
        std::wstring_convert<std::codecvt_utf16<char32_t>, char32_t> converter;
        std::string bytes = converter.to_bytes(reinterpret_cast<const char32_t *>(str.data()));
        return std::u32string(reinterpret_cast<const char32_t *>(bytes.data()), bytes.size() / 4);
    }
    
    std::u16string wide_to_utf16(const std::wstring &str)
    {
        if (sizeof(wchar_t) == 2)
            return std::u16string(reinterpret_cast<const char16_t *>(str.data()));
    
        std::wstring_convert<std::codecvt_utf16<wchar_t>> converter;
        std::string bytes = converter.to_bytes(str.data());
        return std::u16string(reinterpret_cast<const char16_t *>(bytes.data()), bytes.size() / 2);
    }

    template <class T>
    bool equal_ignore_case(const std::basic_string<T> &str1, const std::basic_string<T> &str2)
    {
        std::locale loc;

        auto lstr1 = std::tolower(str1, loc);
        auto lstr2 = std::tolower(str2, loc);

        return lstr1 == lstr2;
    }

}
