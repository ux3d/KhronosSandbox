#ifndef RENDER_HELPERACCESSRESOURCE_H_
#define RENDER_HELPERACCESSRESOURCE_H_

#include "../common/Common.h"
#include "../gltf/GLTF.h"

#include "AllocationManager.h"

class HelperAccessResource {
public:

	static VkBuffer getBuffer(AllocationManager& resourceManager, const BufferView& bufferView);
	static VkBuffer getBuffer(AllocationManager& resourceManager, const Accessor& accessor);

	static VkImage getImage(AllocationManager& resourceManager, const Texture& texture);

	static VkImageView getImageView(AllocationManager& resourceManager, const Texture& texture);

	static VkSampler getSampler(AllocationManager& resourceManager, const Texture& texture);
};

#endif /* RENDER_HELPERACCESSRESOURCE_H_ */
