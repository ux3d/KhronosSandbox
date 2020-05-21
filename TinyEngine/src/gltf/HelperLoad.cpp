#include "HelperLoad.h"

#include <cstdint>

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include <tiny_gltf.h>

#include "HelperAccess.h"

HelperLoad::HelperLoad() :
	model()
{
}

bool HelperLoad::initBuffers(GLTF& glTF)
{
	glTF.buffers.resize(model.buffers.size());

	for (size_t i = 0; i < model.buffers.size(); i++)
	{
		Buffer& buffer = glTF.buffers[i];

		buffer.uri = model.buffers[i].uri;

		buffer.byteLength = static_cast<uint32_t>(model.buffers[i].data.size());

		buffer.binary.resize(buffer.byteLength);
		memcpy(buffer.binary.data(), model.buffers[i].data.data(), buffer.byteLength);
	}

	return true;
}

bool HelperLoad::initBufferViews(GLTF& glTF)
{
	glTF.bufferViews.resize(model.bufferViews.size());

	for (size_t i = 0; i < glTF.bufferViews.size(); i++)
	{
		BufferView& bufferView = glTF.bufferViews[i];

		bufferView.buffer = model.bufferViews[i].buffer;
		if (bufferView.buffer >= 0)
		{
			bufferView.pBuffer = &glTF.buffers[bufferView.buffer];
		}
		else
		{
			return false;
		}

		bufferView.byteOffset = static_cast<uint32_t>(model.bufferViews[i].byteOffset);
		bufferView.byteLength = static_cast<uint32_t>(model.bufferViews[i].byteLength);
		bufferView.byteStride = static_cast<uint32_t>(model.bufferViews[i].byteStride);

		bufferView.target = model.bufferViews[i].target;
	}

	return true;
}

bool HelperLoad::initAccessors(GLTF& glTF)
{
	glTF.accessors.resize(model.accessors.size());

	for (size_t i = 0; i < glTF.accessors.size(); i++)
	{
		Accessor& accessor = glTF.accessors[i];

		accessor.bufferView = model.accessors[i].bufferView;
		if (accessor.bufferView >= 0)
		{
			accessor.pBufferView = &glTF.bufferViews[accessor.bufferView];
		}
		else
		{
			return false;
		}

		accessor.byteOffset = static_cast<uint32_t>(model.accessors[i].byteOffset);

		accessor.count = static_cast<uint32_t>(model.accessors[i].count);

		accessor.componentType = model.accessors[i].componentType;
		switch (accessor.componentType)
		{
			case TINYGLTF_COMPONENT_TYPE_BYTE:
				accessor.componentTypeSize = 1;
				accessor.componentTypeSigned = true;
				accessor.componentTypeInteger = true;
			break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
				accessor.componentTypeSize = 1;
				accessor.componentTypeSigned = false;
				accessor.componentTypeInteger = true;
			break;
			case TINYGLTF_COMPONENT_TYPE_SHORT:
				accessor.componentTypeSize = 2;
				accessor.componentTypeSigned = true;
				accessor.componentTypeInteger = true;
			break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
				accessor.componentTypeSize = 2;
				accessor.componentTypeSigned = false;
				accessor.componentTypeInteger = true;
			break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
				accessor.componentTypeSize = 4;
				accessor.componentTypeSigned = false;
				accessor.componentTypeInteger = true;
			break;
			case TINYGLTF_COMPONENT_TYPE_FLOAT:
				accessor.componentTypeSize = 4;
				accessor.componentTypeSigned = true;
				accessor.componentTypeInteger = false;
			break;
		}

		accessor.type = model.accessors[i].type;
		switch (accessor.type)
		{
			case TINYGLTF_TYPE_SCALAR:
				accessor.typeCount = 1;
			break;
			case TINYGLTF_TYPE_VEC2:
				accessor.typeCount = 2;
			break;
			case TINYGLTF_TYPE_VEC3:
				accessor.typeCount = 3;
			break;
			case TINYGLTF_TYPE_VEC4:
				accessor.typeCount = 4;
			break;
			case TINYGLTF_TYPE_MAT2:
				accessor.typeCount = 4;
			break;
			case TINYGLTF_TYPE_MAT3:
				accessor.typeCount = 9;
			break;
			case TINYGLTF_TYPE_MAT4:
				accessor.typeCount = 16;
			break;
		}
	}

	return true;
}

bool HelperLoad::initImages(GLTF& glTF, const std::string& path)
{
	glTF.images.resize(model.images.size());

	for (size_t i = 0; i < glTF.images.size(); i++)
	{
		Image& image = glTF.images[i];

		image.uri = model.images[i].uri;
		if (!ImageDataIO::open(image.imageDataResources, path + image.uri))
		{
			return false;
		}
	}

	return true;
}

bool HelperLoad::initSamplers(GLTF& glTF)
{
	glTF.samplers.resize(model.samplers.size());

	for (size_t i = 0; i < glTF.samplers.size(); i++)
	{
		Sampler& sampler = glTF.samplers[i];

		switch (model.samplers[i].magFilter)
		{
			case 9728: //NEAREST
				sampler.magFilter = VK_FILTER_NEAREST;
				break;
			case 9729: //LINEAR
				sampler.magFilter = VK_FILTER_LINEAR;
				break;
		}

		switch (model.samplers[i].minFilter)
		{
			case 9728: //NEAREST
				sampler.minFilter = VK_FILTER_NEAREST;
				sampler.maxLod = 0.25f;	// see Mapping of OpenGL to Vulkan filter modes
				 break;
			case 9729: //LINEAR
				sampler.minFilter = VK_FILTER_LINEAR;
				sampler.maxLod = 0.25f;
				break;
			case 9984: //NEAREST_MIPMAP_NEAREST
				sampler.minFilter = VK_FILTER_NEAREST;
				sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
				break;
			case 9985: //LINEAR_MIPMAP_NEAREST
				sampler.minFilter = VK_FILTER_LINEAR;
				sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
				break;
			case 9986: //NEAREST_MIPMAP_LINEAR
				sampler.minFilter = VK_FILTER_NEAREST;
				sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
				break;
			case 9987: //LINEAR_MIPMAP_LINEAR
				sampler.minFilter = VK_FILTER_LINEAR;
				sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
				break;
		}

		switch (model.samplers[i].wrapS)
		{
			case 33071:	//CLAMP_TO_EDGE
				sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
				break;
			case 33648:	//MIRRORED_REPEAT
				sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
				break;
			case 10497:	//REPEAT
				sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				break;
		}
		switch (model.samplers[i].wrapT)
		{
			case 33071:	//CLAMP_TO_EDGE
				sampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
				break;
			case 33648:	//MIRRORED_REPEAT
				sampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
				break;
			case 10497:	//REPEAT
				sampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				break;
		}
	}

	return true;
}

bool HelperLoad::initTextures(GLTF& glTF)
{
	glTF.textures.resize(model.textures.size());

	for (size_t i = 0; i < glTF.textures.size(); i++)
	{
		Texture& texture = glTF.textures[i];

		texture.source = model.textures[i].source;
		texture.sampler = model.textures[i].sampler;
	}

	return true;
}

bool HelperLoad::initMaterials(GLTF& glTF)
{
	glTF.materials.resize(model.materials.size());

	for (size_t i = 0; i < glTF.materials.size(); i++)
	{
		Material& material = glTF.materials[i];

		// Metallic Roughness
		for (const auto& parameterValue : model.materials[i].values)
		{
			if (parameterValue.first == "baseColorTexture")
			{
				material.pbrMetallicRoughness.baseColorTexture.index = parameterValue.second.TextureIndex();
				material.pbrMetallicRoughness.baseColorTexture.texCoord = parameterValue.second.TextureTexCoord();
			}

			if (parameterValue.first == "baseColorFactor")
			{
				material.pbrMetallicRoughness.baseColorFactor = glm::vec4(parameterValue.second.ColorFactor()[0], parameterValue.second.ColorFactor()[1], parameterValue.second.ColorFactor()[2], parameterValue.second.ColorFactor()[3]);
			}

			if (parameterValue.first == "metallicRoughnessTexture")
			{
				material.pbrMetallicRoughness.metallicRoughnessTexture.index = parameterValue.second.TextureIndex();
				material.pbrMetallicRoughness.metallicRoughnessTexture.texCoord = parameterValue.second.TextureTexCoord();
			}

			if (parameterValue.first == "metallicFactor")
			{
				material.pbrMetallicRoughness.metallicFactor = static_cast<float>(parameterValue.second.Factor());
			}

			if (parameterValue.first == "roughnessFactor")
			{
				material.pbrMetallicRoughness.roughnessFactor = static_cast<float>(parameterValue.second.Factor());
			}
		}

		// Base Material
		for (const auto& parameterValue : model.materials[i].additionalValues)
		{
			if (parameterValue.first == "emissiveTexture")
			{
				material.emissiveTexture.index = parameterValue.second.TextureIndex();
				material.emissiveTexture.texCoord = parameterValue.second.TextureTexCoord();
			}

			if (parameterValue.first == "emissiveFactor")
			{
				material.emissiveFactor = glm::vec3(parameterValue.second.ColorFactor()[0], parameterValue.second.ColorFactor()[1], parameterValue.second.ColorFactor()[2]);
			}

			if (parameterValue.first == "occlusionTexture")
			{
				material.occlusionTexture.index = parameterValue.second.TextureIndex();
				material.occlusionTexture.texCoord = parameterValue.second.TextureTexCoord();

				material.occlusionTexture.strength = static_cast<float>(parameterValue.second.TextureStrength());
			}

			if (parameterValue.first == "normalTexture")
			{
				material.normalTexture.index = parameterValue.second.TextureIndex();
				material.normalTexture.texCoord = parameterValue.second.TextureTexCoord();

				material.normalTexture.scale = static_cast<float>(parameterValue.second.TextureScale());
			}

		}

		if (model.materials[i].alphaMode == "OPAQUE")
		{
			material.alphaMode = 0;
		}
		else if (model.materials[i].alphaMode == "MASK")
		{
			material.alphaMode = 1;
		}
		else if (model.materials[i].alphaMode == "BLEND")
		{
			material.alphaMode = 2;
		}

		material.alphaCutoff = static_cast<float>(model.materials[i].alphaCutoff);

		material.doubleSided = model.materials[i].doubleSided;
	}

	return true;
}

bool HelperLoad::initMeshes(GLTF& glTF)
{
	glTF.meshes.resize(model.meshes.size());

	for (size_t i = 0; i < glTF.meshes.size(); i++)
	{
		Mesh& mesh = glTF.meshes[i];

		mesh.primitives.resize(model.meshes[i].primitives.size());

		for (size_t k = 0; k < mesh.primitives.size(); k++)
		{
			Primitive& primitive = mesh.primitives[k];

			primitive.indices = model.meshes[i].primitives[k].indices;
			primitive.material = model.meshes[i].primitives[k].material;

			primitive.attributesCount = static_cast<uint32_t>(model.meshes[i].primitives[k].attributes.size());

			for (const auto& attribute : model.meshes[i].primitives[k].attributes)
			{
				uint32_t accessorIndex = attribute.second;

				if (attribute.first == "POSITION")
				{
					primitive.position = accessorIndex;
				}
				else if (attribute.first == "NORMAL")
				{
					primitive.normal = accessorIndex;
				}
				else if (attribute.first == "TANGENT")
				{
					primitive.tangent = accessorIndex;
				}
				else if (attribute.first == "TEXCOORD_0")
				{
					primitive.texCoord0 = accessorIndex;
				}
				else if (attribute.first == "TEXCOORD_1")
				{
					primitive.texCoord1 = accessorIndex;
				}
				else if (attribute.first == "COLOR_0")
				{
					primitive.color0 = accessorIndex;
				}
				else if (attribute.first == "JOINTS_0")
				{
					primitive.joints0 = accessorIndex;
				}
				else if (attribute.first == "JOINTS_1")
				{
					primitive.joints1 = accessorIndex;
				}
				else if (attribute.first == "WEIGHTS_0")
				{
					primitive.weights0 = accessorIndex;
				}
				else if (attribute.first == "WEIGHTS_1")
				{
					primitive.weights1 = accessorIndex;
				}
				else
				{
					// Not supported.

					return false;
				}
			}

			if (primitive.position < 0)
			{
				return false;
			}
		}
	}

	return true;
}

bool HelperLoad::initNodes(GLTF& glTF)
{
	glTF.nodes.resize(model.nodes.size());

	for (size_t i = 0; i < glTF.nodes.size(); i++)
	{
		Node& node = glTF.nodes[i];

		if (model.nodes[i].translation.size())
		{
			node.translation = glm::vec3(model.nodes[i].translation[0], model.nodes[i].translation[1], model.nodes[i].translation[2]);
		}
		if (model.nodes[i].scale.size())
		{
			node.scale = glm::vec3(model.nodes[i].scale[0], model.nodes[i].scale[1], model.nodes[i].scale[2]);
		}
		if (model.nodes[i].rotation.size())
		{
			node.rotation = glm::quat((float)model.nodes[i].rotation[3], (float)model.nodes[i].rotation[0], (float)model.nodes[i].rotation[1], (float)model.nodes[i].rotation[2]);
		}
		if (model.nodes[i].matrix.size())
		{
			glm::mat4 matrix = glm::mat4(	model.nodes[i].matrix[0], model.nodes[i].matrix[1], model.nodes[i].matrix[2], model.nodes[i].matrix[3],
											model.nodes[i].matrix[4], model.nodes[i].matrix[5], model.nodes[i].matrix[6], model.nodes[i].matrix[7],
											model.nodes[i].matrix[8], model.nodes[i].matrix[9], model.nodes[i].matrix[10], model.nodes[i].matrix[11],
											model.nodes[i].matrix[12], model.nodes[i].matrix[13], model.nodes[i].matrix[14], model.nodes[i].matrix[15]
			);

			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(matrix, node.scale, node.rotation, node.translation, skew, perspective);
		}

		if (model.nodes[i].mesh >= 0)
		{
			node.mesh = model.nodes[i].mesh;
		}

		for (size_t k = 0; k < model.nodes[i].children.size(); k++)
		{
			node.children.push_back(model.nodes[i].children[k]);
		}
	}

	return true;
}

bool HelperLoad::initScenes(GLTF& glTF)
{
	glTF.scenes.resize(model.scenes.size());

	for (size_t i = 0; i < glTF.scenes.size(); i++)
	{
		Scene& scene = glTF.scenes[i];

		for (size_t k = 0; k < model.scenes[i].nodes.size(); k++)
		{
			scene.nodes.push_back(model.scenes[i].nodes[k]);
		}
	}

	return true;
}

bool HelperLoad::open(GLTF& glTF, const std::string& filename)
{
	// Load glTF

	std::string err = "";
	std::string warn = "";

	tinygltf::TinyGLTF tinyGLTF;
	bool status = tinyGLTF.LoadASCIIFromFile(&model, &err, &warn, filename);
	if (!status)
	{
		return false;
	}

	// Images

	std::string path = HelperFile::getPath(filename);
	if (!initImages(glTF, path))
	{
		return false;
	}

	// Samplers

	if (!initSamplers(glTF))
	{
		return false;
	}

	// Textures

	if (!initTextures(glTF))
	{
		return false;
	}

	// Materials

	if (!initMaterials(glTF))
	{
		return false;
	}

	// Buffers

	if (!initBuffers(glTF))
	{
		return false;
	}

	// BufferViews

	if (!initBufferViews(glTF))
	{
		return false;
	}

	// Accessors

	if (!initAccessors(glTF))
	{
		return false;
	}

	// Meshes

	if (!initMeshes(glTF))
	{
		return false;
	}

	// Nodes

	if (!initNodes(glTF))
	{
		return false;
	}

	// Scenes

	if (!initScenes(glTF))
	{
		return false;
	}

	// Default scene

	if (model.defaultScene >= 0)
	{
		glTF.defaultScene = model.defaultScene;
	}

	return true;
}
