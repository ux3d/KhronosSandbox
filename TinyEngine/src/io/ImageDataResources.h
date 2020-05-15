#ifndef IO_IMAGEDATARESOURCES_H_
#define IO_IMAGEDATARESOURCES_H_

#include <vector>

#include "../common/Common.h"

struct ImageDataResource {
	std::vector<uint8_t> pixels;
	uint32_t width = 0;
	uint32_t height = 0;
	VkFormat format = VK_FORMAT_UNDEFINED;

	uint32_t mipLevel = 0;
	uint32_t face = 0;
};

struct ImageDataResources {
	std::vector<ImageDataResource> images = std::vector<ImageDataResource>(1);
	uint32_t mipLevels = 1;
	uint32_t faceCount = 1;
};

#endif /* IO_IMAGEDATARESOURCES_H_ */
