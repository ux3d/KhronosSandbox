#ifndef RESOURCE_RESOURCEMANAGER_H_
#define RESOURCE_RESOURCEMANAGER_H_

#include <cstdint>
#include <map>

#include "../composite/Composite.h"

#include "BufferViewResource.h"
#include "MaterialResource.h"
#include "PrimitiveResource.h"
#include "SceneResource.h"
#include "WorldResource.h"

class ResourceManager {

private:

	std::map<uint64_t, BufferViewResource> bufferViewResources;
	std::map<uint64_t, TextureResource> textureResources;
	std::map<uint64_t, MaterialResource> materialResources;
	std::map<uint64_t, PrimitiveResource> primitiveResources;
	std::map<uint64_t, SceneResource> sceneResources;
	std::map<uint64_t, WorldResource> worldResources;

	void terminate(BufferViewResource& bufferViewResource, VkDevice device);
	void terminate(TextureResource& textureResource, VkDevice device);
	void terminate(MaterialResource& materialResource, VkDevice device);
	void terminate(PrimitiveResource& primitiveResource, VkDevice device);
	void terminate(SceneResource& sceneResource, VkDevice device);
	void terminate(WorldResource& worldResource, VkDevice device);

public:

	ResourceManager();

	~ResourceManager();

	//

	BufferViewResource* getBufferViewResource(uint64_t bufferViewHandle);
	TextureResource* getTextureResource(uint64_t textureHandle);
	MaterialResource* getMaterialResource(uint64_t materialHandle);
	PrimitiveResource* getPrimitiveResource(uint64_t primitiveHandle);
	SceneResource* getSceneResource(uint64_t sceneHandle);
	WorldResource* getWorldResource(uint64_t worldHandle);

	//

	bool resetBufferView(uint64_t bufferViewHandle, VkDevice device);
	bool resetTexture(uint64_t textureHandle, VkDevice device);
	bool resetMaterial(uint64_t materialHandle, VkDevice device);
	bool resetPrimitive(uint64_t primitiveHanlde, VkDevice device);
	bool resetScene(uint64_t sceneHandle, VkDevice device);
	bool resetWorld(uint64_t worldHandle, VkDevice device);

	void terminate(VkDevice device);

};

#endif /* RESOURCE_RESOURCEMANAGER_H_ */
