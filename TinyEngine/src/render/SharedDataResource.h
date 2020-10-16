#ifndef RENDER_SHAREDDATARESOURCE_H_
#define RENDER_SHAREDDATARESOURCE_H_

#include "../composite/Composite.h"

#include "BaseResource.h"

struct SharedDataResource : BaseResource {

	VertexBufferResourceCreateInfo vertexBufferResourceCreateInfo = {};
	UniformBufferResourceCreateInfo uniformBufferResourceCreateInfo = {};
	StorageBufferResourceCreateInfo storageBufferResourceCreateInfo = {};

	VertexBufferResource vertexBufferResource = {};
	UniformBufferResource uniformBufferResource = {};
	StorageBufferResource storageBufferResource = {};

	VkBufferUsageFlags usage = 0;
	VkDeviceSize size = 0;

};

#endif /* RENDER_SHAREDDATARESOURCE_H_ */
