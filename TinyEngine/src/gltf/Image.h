#ifndef GLTF_IMAGE_H_
#define GLTF_IMAGE_H_

#include <string>

#include "../io/IO.h"

class Image {
public:
	std::string uri = "";

	// Generic helper

	ImageDataResources imageDataResources = {};
};

#endif /* GLTF_IMAGE_H_ */
