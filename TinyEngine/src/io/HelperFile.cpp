#include "HelperFile.h"

#include <algorithm>
#include <cstdio>

std::string HelperFile::getPath(const std::string& filename)
{
	size_t position = filename.rfind("/");
	if (position == std::string::npos)
	{
		position = filename.rfind("\\");
	}

	if (position != std::string::npos)
	{
		return filename.substr(0, position + 1);
	}

	return "";
}

std::string HelperFile::getExtension(const std::string& filename)
{
	size_t position = filename.rfind(".");

	if (position != std::string::npos)
	{
		std::string result = filename.substr(position + 1);
		std::transform(result.begin(), result.end(), result.begin(), ::tolower);
		return result;
	}

	return "";
}

bool HelperFile::exists(const std::string& filename)
{
	FILE* file = fopen(filename.c_str(), "rb");

	if (file)
	{
		fclose(file);
		return true;
	}

	return false;
}

