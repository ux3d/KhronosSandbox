#ifndef FILEIO_H_
#define FILEIO_H_

#include "Helper.h"

#include <string>

class FileIO
{
public:

	static std::string getPath(const std::string& filename);

	static std::string getExtension(const std::string& filename);

	static bool openText(std::string& output, const std::string& filename);

	static bool openImageData(ImageDataResources& output, const std::string& filename, const uint32_t channels = 4);
};

#endif /* FILEIO_H_ */
