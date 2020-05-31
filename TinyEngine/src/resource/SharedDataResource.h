#ifndef RESOURCE_SHAREDDATARESOURCE_H_
#define RESOURCE_SHAREDDATARESOURCE_H_

#include "../composite/Composite.h"

struct SharedDataResource {

	// Mapper helper

	VertexBufferResourceCreateInfo vertexBufferResourceCreateInfo = {};

	// Rasterize helper

	VertexBufferResource vertexBufferResource = {};
};

#endif /* RESOURCE_SHAREDDATARESOURCE_H_ */
