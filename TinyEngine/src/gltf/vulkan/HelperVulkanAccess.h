#ifndef GLTF_HELPERVULKANACCESS_H_
#define GLTF_HELPERVULKANACCESS_H_

#include "../GLTF.h"

class HelperVulkanAccess {
public:

	static VkBuffer getBuffer(const BufferView& bufferView);

	static VkBuffer getBuffer(const Accessor& accessor);
};

#endif /* GLTF_HELPERVULKANACCESS_H_ */
