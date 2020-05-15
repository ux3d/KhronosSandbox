#ifndef GLTF_GLTF_H_
#define GLTF_GLTF_H_

#include <vector>

#include "Image.h"

#include "Sampler.h"

#include "Texture.h"

#include "TextureInfo.h"

#include "PbrMetallicRoughness.h"

#include "Material.h"

#include "Buffer.h"

#include "BufferView.h"

#include "Accessor.h"

#include "Primitive.h"

#include "Mesh.h"

#include "Node.h"

#include "Scene.h"

#include "HelperAccess.h"

#include "HelperLoad.h"

#include "HelperUpdate.h"

class GLTF {
public:
	std::vector<Image> images;
	std::vector<Sampler> samplers;
	std::vector<Texture> textures;
	std::vector<Material> materials;
	std::vector<Buffer> buffers;
	std::vector<BufferView> bufferViews;
	std::vector<Accessor> accessors;
	std::vector<Mesh> meshes;
	std::vector<Node> nodes;
	std::vector<Scene> scenes;
	size_t defaultScene = 0;
};

#endif /* GLTF_GLTF_H_ */
