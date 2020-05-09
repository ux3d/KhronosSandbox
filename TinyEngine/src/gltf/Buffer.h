#ifndef GLTF_BUFFER_H_
#define GLTF_BUFFER_H_

#include "../TinyEngine.h"

class Buffer {
public:
	std::string uri = "";
	size_t byteLength = 1;

	// Rasterize helper

	std::vector<uint8_t> binary;
};

#endif /* GLTF_BUFFER_H_ */
