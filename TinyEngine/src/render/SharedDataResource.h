#ifndef RENDER_SHAREDDATARENDER_H_
#define RENDER_SHAREDDATARENDER_H_

#include "../composite/Composite.h"

#include "BaseResource.h"

struct SharedDataResource : BaseResource {

	// Mapper helper

	VertexBufferResourceCreateInfo vertexBufferResourceCreateInfo = {};
	UniformBufferResourceCreateInfo uniformBufferResourceCreateInfo = {};
	StorageBufferResourceCreateInfo storageBufferResourceCreateInfo = {};

	// Rasterize helper

	VertexBufferResource vertexBufferResource = {};
	UniformBufferResource uniformBufferResource = {};
	StorageBufferResource storageBufferResource = {};

};

#endif /* RENDER_SHAREDDATARENDER_H_ */
