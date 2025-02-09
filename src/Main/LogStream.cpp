#include "LogStream.h"

namespace main
{

LogStream &LogStream::operator<<(const std::string &str)
{
	std::string str_c = str;

	if (str_c.back() == '\n') {
        str_c.erase(std::prev(str_c.end()));

		Stream << str_c;

		log(Stream.str());
	}
	else
		Stream << str_c;

	return *this;
}

void LogStream::log(const std::string &str)
{
	const char *cstr = str.c_str();

	switch (Type) {
		case LT_WARNING:
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, cstr);
			break;
		case LT_INFO:
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, cstr);
			break;
		case LT_ERROR:
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, cstr);
			break;
	}
}

// Standard log streams
LogStream WarnStream(LT_WARNING);
LogStream InfoStream(LT_INFO);
LogStream ErrorStream(LT_ERROR);

}
