#ifndef COMMON_LOGGER_H_
#define COMMON_LOGGER_H_

#include <cstdint>
#include <vector>

#include "Common.h"

enum DebugLevel {
	TE_ERROR,
	TE_WARNING,
	TE_INFO,
	TE_DEBUG
};

class Logger
{
private:

	static const std::vector<const char *> LOG_STRINGS;

	static const char* reduceFileMacro(const char* fileMacro);

public:

	static DebugLevel debugLevel;

	static void print(DebugLevel debugLevel, const char* fileMacro, uint32_t lineMacro, VkResult result);

	static void print(DebugLevel debugLevel, const char* fileMacro, uint32_t lineMacro, const char* format, ...);

};

#endif /* COMMON_LOGGER_H_ */
