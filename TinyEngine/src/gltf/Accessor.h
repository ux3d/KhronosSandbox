#ifndef GLTF_ACCESSOR_H_
#define GLTF_ACCESSOR_H_

#include <cstdint>

#include "BufferView.h"

struct AccessorSparseValues {
	int32_t bufferView = -1;
	uint32_t byteOffset = 0;

	// Generic helper

	BufferView* pBufferView = nullptr;
};

struct AccessorSparseIndices {
	int32_t bufferView = -1;
	uint32_t byteOffset = 0;
	int32_t componentType = -1;

	// Generic helper

	uint32_t componentTypeSize = 0;
	bool componentTypeSigned = false;
	bool componentTypeInteger = false;

	BufferView* pBufferView = nullptr;
};

struct AccessorSparse {
	uint32_t count = 0;
	AccessorSparseIndices indices;
	AccessorSparseValues values;

	// Generic helper

	Buffer buffer;
	BufferView bufferView;
};

struct Accessor {
	int32_t bufferView = -1;
	uint32_t byteOffset = 0;
	bool normalized = false;
	uint32_t count = 1;
	int32_t componentType = -1;
	int32_t type = -1;

	AccessorSparse sparse;

	// Generic helper

	uint32_t componentTypeSize = 0;
	bool componentTypeSigned = false;
	bool componentTypeInteger = false;

	uint32_t typeCount = 0;

	BufferView* pBufferView = nullptr;

	Buffer aliasedBuffer;
	BufferView aliasedBufferView;
};

#endif /* GLTF_ACCESSOR_H_ */
