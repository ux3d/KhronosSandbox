#ifndef GLTF_BUFFER_H_
#define GLTF_BUFFER_H_

#include <string>
#include <vector>

struct Buffer {
	std::string uri = "";
	uint32_t byteLength = 0;

	// Generic helper

	std::vector<uint8_t> binary;
};

#endif /* GLTF_BUFFER_H_ */
