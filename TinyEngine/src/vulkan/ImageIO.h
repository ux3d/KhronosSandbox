#ifndef IMAGEIO_H_
#define IMAGEIO_H_

#include <string>
#include "HelperVulkanResource.h"

class ImageIO
{
public:

	static bool openImageData(ImageDataResources& output, const std::string& filename, const uint32_t channels = 4);
};

#endif /* IMAGEIO_H_ */
