#ifndef GLTF_GLTF_H_
#define GLTF_GLTF_H_

#include "../TinyEngine.h"

struct ViewProjectionUniformPushConstant {
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
};

struct ModelViewProjectionUniformPushConstant {
	ViewProjectionUniformPushConstant viewProjection = {};
	glm::mat4 world = glm::mat4(1.0f);
};

struct InverseViewProjectionUniformPushConstant {
	glm::mat4 inverseProjection;
	glm::mat4 inverseView;
};

struct MaterialUniformBuffer {
	glm::vec4 baseColorFactor = glm::vec4(1.0f);

	float metallicFactor = 1.0f;
	float roughnessFactor = 1.0f;
	float normalScale = 1.0f;
	float occlusionStrength = 1.0f;

	glm::vec3 emissiveFactor = glm::vec3(0.0f);
	uint32_t alphaMode = 0;

	float alphaCutoff = 0.5f;
	bool doubleSided = false;
};

struct MaterialUniformBufferRaytrace {
	MaterialUniformBuffer materialUniformBuffer;

	int32_t baseColorTexture = -1;
	int32_t metallicRoughnessTexture = -1;

	int32_t emissiveTexture = -1;

	int32_t occlusionTexture = -1;

	int32_t normalTexture = -1;

	//

	int32_t padding;
};

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

	// Rasterize helper

	ViewProjectionUniformPushConstant viewProjection = {};

	TextureResource diffuse;
	TextureResource specular;
	TextureResource lut;

	// Raytrace helper

	InverseViewProjectionUniformPushConstant inverseViewProjection = {};
};

#endif /* GLTF_GLTF_H_ */
