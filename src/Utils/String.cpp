#include "String.h"
#include "Core/LogStream.h"
#include <codecvt>

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
        if (sizeof(wchar_t) == 4) {
            return std::u32string(reinterpret_cast<const char32_t*>(str.data()), str.size());
        }

        // Для систем где wchar_t 2 байта (UTF-16)
        std::u32string result;
        for (size_t i = 0; i < str.size(); ++i) {
            wchar_t wc = str[i];

            if ((wc & 0xFC00) == 0xD800) { // High surrogate
                if (i + 1 < str.size()) {
                    wchar_t low_surrogate = str[i + 1];
                    if ((low_surrogate & 0xFC00) == 0xDC00) { // Low surrogate
                        // Объединяем суррогатную пару
                        char32_t code_point = 0x10000 + ((wc & 0x3FF) << 10) + (low_surrogate & 0x3FF);
                        result.push_back(code_point);
                        ++i; // Пропускаем low surrogate
                        continue;
                    }
                }
            }

            // Одиночный символ BMP или некорректный surrogate
            result.push_back(static_cast<char32_t>(wc));
        }

        return result;
    }

    std::u16string wide_to_utf16(const std::wstring &str)
    {
        if (sizeof(wchar_t) == 2) {
            return std::u16string(reinterpret_cast<const char16_t*>(str.data()), str.size());
        }

        // Для систем где wchar_t 4 байта (UTF-32)
        std::u16string result;
        for (wchar_t wc : str) {
            char32_t code_point = static_cast<char32_t>(wc);

            if (code_point <= 0xFFFF) {
                // Базовая многоязыковая плоскость
                result.push_back(static_cast<char16_t>(code_point));
            } else if (code_point <= 0x10FFFF) {
                // Суррогатная пара для символов за пределами BMP
                code_point -= 0x10000;
                result.push_back(static_cast<char16_t>((code_point >> 10) + 0xD800));    // High surrogate
                result.push_back(static_cast<char16_t>((code_point & 0x3FF) + 0xDC00));  // Low surrogate
            } else {
                // Некорректный код - заменяем на replacement character
                result.push_back(0xFFFD);
            }
        }

        return result;
    }

    std::vector<Uint16> wstring_to_uint16(const std::wstring& wstr) {
        //core::InfoStream << "wstr size: " << (u32)wstr.size() << "\n";
        std::vector<Uint16> result;

        if (sizeof(wchar_t) == 2) {
            // Windows: wchar_t уже UTF-16
            result.reserve(wstr.size() + 1);
            for (wchar_t wc : wstr) {
                result.push_back(static_cast<Uint16>(wc));
            }
        } else {
            // Linux/macOS: wchar_t - UTF-32, конвертируем в UTF-16
            for (wchar_t wc : wstr) {
                char32_t code_point = static_cast<char32_t>(wc);

                if (code_point <= 0xFFFF) {
                    result.push_back(static_cast<Uint16>(code_point));
                } else if (code_point <= 0x10FFFF) {
                    code_point -= 0x10000;
                    result.push_back(static_cast<Uint16>((code_point >> 10) + 0xD800));    // High surrogate
                    result.push_back(static_cast<Uint16>((code_point & 0x3FF) + 0xDC00));  // Low surrogate
                } else {
                    result.push_back(0xFFFD); // replacement character
                }
            }
        }

        result.push_back(0); // null terminator для SDL

        return result;
    }

    std::wstring uint16_to_wstring(const std::vector<Uint16>& vec) {
        std::wstring result;

        if (vec.empty() || (vec.size() == 1 && vec[0] == 0)) {
            return L"";
        }

        if (sizeof(wchar_t) == 2) {
            // Windows: wchar_t - UTF-16, просто копируем
            result.reserve(vec.size());
            for (size_t i = 0; i < vec.size(); ++i) {
                Uint16 code = vec[i];
                if (code == 0) break;
                result += static_cast<wchar_t>(code);
            }
        } else {
            // Linux/macOS: wchar_t - UTF-32, конвертируем из UTF-16
            result.reserve(vec.size()); // Обычно меньше или равно

            for (size_t i = 0; i < vec.size(); ++i) {
                Uint16 code = vec[i];

                if (code == 0) break;

                if ((code & 0xFC00) == 0xD800) {
                    // High surrogate
                    if (i + 1 < vec.size()) {
                        Uint16 low_surrogate = vec[i + 1];
                        if ((low_surrogate & 0xFC00) == 0xDC00) {
                            // Valid surrogate pair
                            char32_t code_point = 0x10000 +
                                                  ((code & 0x3FF) << 10) +
                                                  (low_surrogate & 0x3FF);
                            result += static_cast<wchar_t>(code_point);
                            ++i; // Skip the low surrogate
                            continue;
                        }
                    }
                    // Invalid surrogate - fall through to replacement
                } else if ((code & 0xFC00) == 0xDC00) {
                    // Unexpected low surrogate - use replacement
                    result += static_cast<wchar_t>(0xFFFD);
                    continue;
                }

                // Regular BMP character or invalid high surrogate without pair
                result += static_cast<wchar_t>(code);
            }
        }

        return result;
    }

}
