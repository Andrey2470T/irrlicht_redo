#pragma once

#include "BasicIncludes.h"
#include <codecvt>
#include <locale>

namespace utils
{

std::wstring utf8_to_wide(std::string str)
{
	wstring_convert<codecvt_utf8<wchar_t>> converter;
	return converter.from_bytes(str);
}

std::wstring wide_to_utf8(std::wstring str)
{
	wstring_convert<codecvt_utf8<wchar_t>> converter;
	return converter.to_bytes(str);
}

}
