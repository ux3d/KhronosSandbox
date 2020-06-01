#ifndef RENDER_SHAREDDATARENDER_H_
#define RENDER_SHAREDDATARENDER_H_

#include "../composite/Composite.h"

#include "BaseResource.h"

struct SharedDataResource : BaseResource {

	// Mapper helper

	VertexBufferResourceCreateInfo vertexBufferResourceCreateInfo = {};

	// Rasterize helper

	VertexBufferResource vertexBufferResource = {};
};

#endif /* RENDER_SHAREDDATARENDER_H_ */
