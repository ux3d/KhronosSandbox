#ifndef GLTF_BUFFERVIEW_H_
#define GLTF_BUFFERVIEW_H_

#include "../TinyEngine.h"

#include "Buffer.h"

class BufferView {
public:
	int32_t buffer = -1;

	size_t byteOffset = 0;
	size_t byteLength = 1;
	size_t byteStride = 4;

	int32_t target = -1;

	// Generic helper

	Buffer* pBuffer = nullptr;

	// Rasterize helper

	VertexBufferResource vertexBufferResource = {};
};

#endif /* GLTF_BUFFERVIEW_H_ */
