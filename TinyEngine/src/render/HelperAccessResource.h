#ifndef RENDER_HELPERACCESSRESOURCE_H_
#define RENDER_HELPERACCESSRESOURCE_H_

#include "../common/Common.h"

#include "../gltf/GLTF.h"

#include "ResourceManager.h"

class HelperAccessResource {
public:

	static VkBuffer getBuffer(ResourceManager& resourceManager, const BufferView& bufferView);
	static VkBuffer getBuffer(ResourceManager& resourceManager, const Accessor& accessor);

	static VkImage getImage(ResourceManager& resourceManager, const Texture& texture);

	static VkImageView getImageView(ResourceManager& resourceManager, const Texture& texture);

	static VkSampler getSampler(ResourceManager& resourceManager, const Texture& texture);
};

#endif /* RENDER_HELPERACCESSRESOURCE_H_ */
