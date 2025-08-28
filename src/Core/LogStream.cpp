#include "LogStream.h"
#include "IEventReceiver.h"
#include "Events.h"

namespace core
{

void IrrLogStream::setEventReceiver(IEventReceiver *receiver)
{
    Receiver = receiver;
}

IrrLogStream &IrrLogStream::operator<<(const std::string &str)
{
	std::string str_c = str;

    auto it = std::find(str_c.begin(), str_c.end(), '\n');

    if (it != str_c.end()) {
        u32 pos = std::distance(str_c.begin(), it);
		Stream << str_c.substr(0, pos);

		log(Stream.str());
        
        Stream.str("");
        Stream.clear();
	}
	else
		Stream << str_c;

	return *this;
}

void IrrLogStream::log(const std::string &str)
{
	const char *cstr = str.c_str();

    if (Receiver) {
        Event event;
        event.Type = ET_LOG_TEXT_EVENT;
        event.Log.Text = cstr;
        event.Log.Level = Level;

        if (Receiver->OnEvent(event))
            return;
    }

    switch (Level) {
        case LL_ERROR:
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, cstr);
            break;
        case LL_WARNING:
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, cstr);
			break;
        case LL_ACTION:
        case LL_INFO:
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, cstr);
			break;
        case LL_VERBOSE:
            SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, cstr);
            break;
        case LL_TRACE:
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, cstr);
            break;
        default:
            break;
	}
}

// Standard log streams
IrrLogStream WarnStream(LL_WARNING);
IrrLogStream InfoStream(LL_INFO);
IrrLogStream ErrorStream(LL_ERROR);

}
