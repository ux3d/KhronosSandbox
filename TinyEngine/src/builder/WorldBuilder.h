#ifndef BUILDER_WORLDBUILDER_H_
#define BUILDER_WORLDBUILDER_H_

#include <cstdint>
#include <vector>

#include "../composite/Composite.h"
#include "../gltf/GLTF.h"

#include "../render/Render.h"

class WorldBuilder {

private:

	RenderManager& renderManager;

	std::vector<uint64_t> textureHandles;
	std::vector<uint64_t> materialHandles;
	std::vector<uint64_t> meshHandles;
	std::vector<uint64_t> nodeHandles;
	uint64_t glTFHandle = 0;

	bool buildBufferViews(const GLTF& glTF);

	bool buildAccessors(const GLTF& glTF);

	bool buildTextures(const GLTF& glTF);

	bool buildMaterials(const GLTF& glTF);

	bool buildMeshes(const GLTF& glTF);

	bool buildNodes(const GLTF& glTF);

	bool buildScene(const GLTF& glTF);

	uint64_t getBufferHandle(const Accessor& accessor);

	bool createSharedDataResource(const BufferView& bufferView);
	bool createSharedDataResource(VkDeviceSize size, const void* data);

public:

	WorldBuilder(RenderManager& resourceManager);

	bool build(const GLTF& glTF, const std::string& diffuseFilename);

	void terminate();

};

#endif /* BUILDER_WORLDBUILDER_H_ */
