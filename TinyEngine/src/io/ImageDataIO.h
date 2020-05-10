#ifndef IO_IMAGEDATAIO_H_
#define IO_IMAGEDATAIO_H_

#include "HelperImageDataResource.h"

#include <string>

class ImageDataIO
{
public:

	static bool openImageData(ImageDataResources& output, const std::string& filename, const uint32_t channels = 4);
};

#endif /* IO_IMAGEDATAIO_H_ */
