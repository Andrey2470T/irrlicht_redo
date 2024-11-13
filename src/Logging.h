#pragma once

#define LOG(msg) \
	std::cout << msg << std::endl;
#define LOG_WITH_HINT(msg, hint) \
	std::cout << "Error:" << msg << ": " << hint << std::endl;
#define ERR_LOG(msg) \
	std::cout << msg << std::endl;
#define ERR_LOG_WITH_HINT(msg, hint) \
	std::cout << "Error:" << msg << ": " << hint << std::endl;
