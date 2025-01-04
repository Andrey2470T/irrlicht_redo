#pragma once

#include "BasicIncludes.h"
#include <codecvt>
#include <locale>

namespace utils
{

std::string utf8_to_wide(std::string str)
{
	string_convert<codecvt_utf8<char_t>> converter;
	return converter.from_bytes(str);
}

std::string wide_to_utf8(std::string str)
{
	string_convert<codecvt_utf8<char_t>> converter;
	return converter.to_bytes(str);
}

}
