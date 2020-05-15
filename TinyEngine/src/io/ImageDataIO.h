#ifndef IO_IMAGEDATAIO_H_
#define IO_IMAGEDATAIO_H_

#include <cstdint>
#include <string>

#include "ImageDataResources.h"

class ImageDataIO
{
public:

	static bool open(ImageDataResources& output, const std::string& filename, const uint32_t channels = 4);
};

#endif /* IO_IMAGEDATAIO_H_ */
