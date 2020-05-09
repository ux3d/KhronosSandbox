#ifndef IMAGEDATAIO_H_
#define IMAGEDATAIO_H_

#include "HelperImageDataResource.h"

#include <string>

class ImageDataIO
{
public:

	static bool openImageData(ImageDataResources& output, const std::string& filename, const uint32_t channels = 4);
};

#endif /* IMAGEDATAIO_H_ */
