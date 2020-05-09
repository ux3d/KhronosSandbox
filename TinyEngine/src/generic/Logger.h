#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <vector>

enum DebugLevel {
	TE_ERROR,
	TE_WARNING,
	TE_INFO,
	TE_DEBUG
};

class Logger
{
private:

	static const std::vector<std::string> LOG_STRINGS;

	static const char* getFilename(const char* fileName);

public:

	static DebugLevel debugLevel;

	static void print(DebugLevel debugLevel, const char* fileName, uint32_t lineNumber, int32_t result);

	static void print(DebugLevel debugLevel, const char* fileName, uint32_t lineNumber, const char* format, ...);

};

#endif /* LOGGER_H_ */
