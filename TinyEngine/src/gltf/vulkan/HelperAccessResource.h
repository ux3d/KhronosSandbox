#ifndef GLTF_HELPERACCESSRESOURCE_H_
#define GLTF_HELPERACCESSRESOURCE_H_

#include "../BufferView.h"
#include "../Accessor.h"

#include "ResourceManager.h"

class HelperAccessResource {
public:

	static VkBuffer getBuffer(ResourceManager& resourceManager, const BufferView* bufferView);

	static VkBuffer getBuffer(ResourceManager& resourceManager, const Accessor* accessor);
};

#endif /* GLTF_HELPERVULKANACCESSRESOURCE_H_ */
