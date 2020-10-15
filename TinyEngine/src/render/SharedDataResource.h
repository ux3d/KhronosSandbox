#ifndef RENDER_SHAREDDATARENDER_H_
#define RENDER_SHAREDDATARENDER_H_

#include "../composite/Composite.h"

#include "BaseResource.h"

struct SharedDataResource : BaseResource {

	VertexBufferResourceCreateInfo vertexBufferResourceCreateInfo = {};
	UniformBufferResourceCreateInfo uniformBufferResourceCreateInfo = {};
	StorageBufferResourceCreateInfo storageBufferResourceCreateInfo = {};

	VertexBufferResource vertexBufferResource = {};
	UniformBufferResource uniformBufferResource = {};
	StorageBufferResource storageBufferResource = {};

};

#endif /* RENDER_SHAREDDATARENDER_H_ */
