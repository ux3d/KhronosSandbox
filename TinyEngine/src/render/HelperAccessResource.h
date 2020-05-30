#ifndef RENDER_HELPERACCESSRESOURCE_H_
#define RENDER_HELPERACCESSRESOURCE_H_

#include "../common/Common.h"
#include "../gltf/GLTF.h"

#include "../render/AllocationManager.h"

class HelperAccessResource {
public:

	static VkBuffer getBuffer(AllocationManager& allocationManager, const BufferView& bufferView);
	static VkBuffer getBuffer(AllocationManager& allocationManager, const Accessor& accessor);

	static VkImage getImage(AllocationManager& allocationManager, const Texture& texture);

	static VkImageView getImageView(AllocationManager& allocationManager, const Texture& texture);

	static VkSampler getSampler(AllocationManager& allocationManager, const Texture& texture);
};

#endif /* RENDER_HELPERACCESSRESOURCE_H_ */
