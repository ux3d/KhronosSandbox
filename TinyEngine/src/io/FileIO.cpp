#include "FileIO.h"

#include <algorithm>
#include <fstream>

std::string FileIO::getPath(const std::string& filename)
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

std::string FileIO::getExtension(const std::string& filename)
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

bool FileIO::open(std::string& output, const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open())
	{
		return false;
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	file.seekg(0);

	output.resize(fileSize);

	file.read(output.data(), fileSize);
	file.close();

	return true;
}
