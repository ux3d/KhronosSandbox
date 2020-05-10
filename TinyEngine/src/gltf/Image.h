#ifndef GLTF_IMAGE_H_
#define GLTF_IMAGE_H_

#include "../TinyEngine.h"

class Image {
public:
	std::string uri = "";

	// Rasterize helper

	ImageDataResources imageDataResources = {};
};

#endif /* GLTF_IMAGE_H_ */
