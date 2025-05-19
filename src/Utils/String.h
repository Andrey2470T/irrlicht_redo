#pragma once

#include "BasicIncludes.h"

namespace utils
{

    std::wstring utf8_to_wide(const std::string &str);
    std::string wide_to_utf8(const std::wstring &str);
    std::u32string wide_to_utf32(const std::wstring &str);
    std::u16string wide_to_utf16(const std::wstring &str);

}
