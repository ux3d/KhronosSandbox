#ifndef GLTF_GLTF_H_
#define GLTF_GLTF_H_

#include <vector>

#include "Buffer.h"
#include "BufferView.h"
#include "Accessor.h"
#include "Image.h"
#include "Sampler.h"
#include "Texture.h"
#include "TextureInfo.h"
#include "Material.h"
#include "PbrMetallicRoughness.h"
#include "Primitive.h"
#include "Mesh.h"
#include "Node.h"
#include "Skin.h"
#include "Animation.h"
#include "Scene.h"
#include "HelperAccess.h"
#include "HelperAnimate.h"
#include "HelperLoad.h"
#include "HelperUpdate.h"

struct GLTF {
	std::vector<Image> images;
	std::vector<Sampler> samplers;
	std::vector<Texture> textures;
	std::vector<Material> materials;
	std::vector<Buffer> buffers;
	std::vector<BufferView> bufferViews;
	std::vector<Accessor> accessors;
	std::vector<Mesh> meshes;
	std::vector<Node> nodes;
	std::vector<Animation> animations;
	std::vector<Skin> skins;
	std::vector<Scene> scenes;
	uint32_t defaultScene = 0;
};

#endif /* GLTF_GLTF_H_ */
