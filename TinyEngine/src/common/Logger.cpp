#include "Logger.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <map>

const char* Logger::reduceFileMacro(const char* fileMacro)
{
	return strrchr(fileMacro, '/') ? strrchr(fileMacro, '/') + 1 : (strrchr(fileMacro, '\\') ? strrchr(fileMacro, '\\') + 1 : fileMacro);
}

const std::vector<const char*> Logger::LOG_STRINGS = {"ERROR", "WARNING", "INFO", "DEBUG"};

DebugLevel Logger::debugLevel = TinyEngine_INFO;

void Logger::print(DebugLevel debugLevel, const char* fileMacro, uint32_t lineMacro, VkResult result)
{
	static std::map<int32_t, const char *> resultToString = {
		{  0, "VK_SUCCESS"},
		{  1, "VK_NOT_READY"},
		{  2, "VK_TIMEOUT"},
		{  3, "VK_EVENT_SET"},
		{  4, "VK_EVENT_RESET"},
		{  5, "VK_INCOMPLETE"},
		{ -1, "VK_ERROR_OUT_OF_HOST_MEMORY"},
		{ -2, "VK_ERROR_OUT_OF_DEVICE_MEMORY"},
		{ -3, "VK_ERROR_INITIALIZATION_FAILED"},
		{ -4, "VK_ERROR_DEVICE_LOST"},
		{ -5, "VK_ERROR_MEMORY_MAP_FAILED"},
		{ -6, "VK_ERROR_LAYER_NOT_PRESENT"},
		{ -7, "VK_ERROR_EXTENSION_NOT_PRESENT"},
		{ -8, "VK_ERROR_FEATURE_NOT_PRESENT"},
		{ -9, "VK_ERROR_INCOMPATIBLE_DRIVER"},
		{ -10, "VK_ERROR_TOO_MANY_OBJECTS"},
		{ -11, "VK_ERROR_FORMAT_NOT_SUPPORTED"},
		{ -12, "VK_ERROR_FRAGMENTED_POOL"},
		{ -13, "VK_ERROR_UNKNOWN"},
		{ -1000069000, "VK_ERROR_OUT_OF_POOL_MEMORY"},
		{ -1000072003, "VK_ERROR_INVALID_EXTERNAL_HANDLE"},
		{ -1000161000, "VK_ERROR_FRAGMENTATION"},
		{ -1000257000, "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS"},
		{ -1000000000, "VK_ERROR_SURFACE_LOST_KHR"},
		{ -1000000001, "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR"},
		{  1000001003, "VK_SUBOPTIMAL_KHR"},
		{ -1000001004, "VK_ERROR_OUT_OF_DATE_KHR"},
		{ -1000003001, "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR"},
		{ -1000011001, "VK_ERROR_VALIDATION_FAILED_EXT"},
		{ -1000012000, "VK_ERROR_INVALID_SHADER_NV"},
		{ -1000150000, "VK_ERROR_INCOMPATIBLE_VERSION_KHR"} ,
		{ -1000158000, "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT"},
		{ -1000174001, "VK_ERROR_NOT_PERMITTED_EXT"},
		{ -1000255000, "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT"},
		{  1000268000, "VK_THREAD_IDLE_KHR"},
		{  1000268001, "VK_THREAD_DONE_KHR"},
		{  1000268002, "VK_OPERATION_DEFERRED_KHR"},
		{  1000268003, "VK_OPERATION_NOT_DEFERRED_KHR"},
		{  1000297000, "VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT"}
	};

	print(debugLevel, fileMacro, lineMacro, "VkResult = %s\n", resultToString[static_cast<int32_t>(result)]);
}

void Logger::print(DebugLevel debugLevel, const char* fileName, uint32_t lineNumber, const char* format, ...)
{
	if (debugLevel > Logger::debugLevel)
	{
		return;
	}

	va_list argList;
	va_start(argList, format);

	char buffer[2048] = { 0 };
	vsnprintf(buffer, 2048, format, argList);

	if (Logger::debugLevel == TinyEngine_DEBUG)
	{
		printf("Log [%s] in '%s' at %u: %s\n", Logger::LOG_STRINGS[debugLevel], reduceFileMacro(fileName), lineNumber, buffer);
	}
	else
	{
		printf("Log [%s]: %s\n", Logger::LOG_STRINGS[debugLevel], buffer);
	}

	va_end(argList);
}
