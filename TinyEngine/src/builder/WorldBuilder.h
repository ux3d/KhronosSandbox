#ifndef BUILDER_WORLDBUILDER_H_
#define BUILDER_WORLDBUILDER_H_

#include <cstdint>
#include <map>
#include <vector>

#include "../composite/Composite.h"
#include "../gltf/GLTF.h"

#include "../render/Render.h"

class WorldBuilder {

private:

	const GLTF& glTF;
	const std::string environment;

	RenderManager& renderManager;

	std::map<const BufferView*, uint64_t> bufferViewToHandle;
	std::map<const Node*, uint64_t> nodeToHandles;

	std::vector<uint64_t> textureHandles;
	std::vector<uint64_t> materialHandles;
	std::vector<uint64_t> groupHandles;
	std::vector<uint64_t> instanceHandles;

	bool buildBufferViews();

	bool buildAccessors();

	bool buildTextures();

	bool buildMaterials();

	bool buildMeshes();

	bool buildNodes();

	bool buildScene();

	uint64_t getBufferHandle(const Accessor& accessor);

	bool createSharedDataResource(const BufferView& bufferView);

public:

	WorldBuilder(const GLTF& glTF, const std::string& environment, RenderManager& resourceManager);

	bool build();

	std::map<const Node*, uint64_t> cloneNodeToHandles() const;

};

#endif /* BUILDER_WORLDBUILDER_H_ */
