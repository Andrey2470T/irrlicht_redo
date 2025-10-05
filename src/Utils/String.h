#pragma once

#include "BasicIncludes.h"
#include <locale>
#include <cctype>

namespace utils
{

    std::wstring utf8_to_wide(const std::string &str);
    std::string wide_to_utf8(const std::wstring &str);
    std::u32string wide_to_utf32(const std::wstring &str);
    std::u16string wide_to_utf16(const std::wstring &str);

    template <class T>
    bool equal_ignore_case(const std::basic_string<T> &str1, const std::basic_string<T> &str2)
    {
        if (str1.size() != str2.size())
            return false;

        std::locale loc;

        for (size_t i = 0; i < str1.size(); ++i) {
            if (std::tolower(str1[i], loc) != std::tolower(str2[i], loc))
                return false;
        }

        return true;
    }

}
