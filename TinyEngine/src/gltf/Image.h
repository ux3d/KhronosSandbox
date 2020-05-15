#ifndef GLTF_IMAGE_H_
#define GLTF_IMAGE_H_

#include <string>

#include "../io/IO.h"

struct Image {
	std::string uri = "";

	// Generic helper

	ImageDataResources imageDataResources = {};
};

#endif /* GLTF_IMAGE_H_ */
