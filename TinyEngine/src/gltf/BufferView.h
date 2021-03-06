#ifndef GLTF_BUFFERVIEW_H_
#define GLTF_BUFFERVIEW_H_

#include <cstdint>

#include "Buffer.h"

struct BufferView {
	int32_t buffer = -1;

	uint32_t byteOffset = 0;
	uint32_t byteLength = 1;
	uint32_t byteStride = 4;

	int32_t target = -1;

	// Generic helper

	Buffer* pBuffer = nullptr;
};

#endif /* GLTF_BUFFERVIEW_H_ */
