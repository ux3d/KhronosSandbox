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

static bool LoadImageData(tinygltf::Image *image, const int image_idx, std::string *err, std::string *warn, int req_width, int req_height, const unsigned char *bytes, int size, void *user_data) {
	ImageDataResources imageDataResources;

	// Defaulting to 4 channels.
	if (!ImageDataIO::open(imageDataResources, bytes, size))
	{
		return false;
	}

	if (imageDataResources.faceCount != 1 || imageDataResources.mipLevels != 1 || imageDataResources.images.size() != 1)
	{
		return false;
	}

	ImageDataResource& imageDataResource = imageDataResources.images[0];

	if (imageDataResource.pixels.size() != imageDataResource.width * imageDataResource.height * 4)
	{
		return false;
	}

	image->width = (int)imageDataResource.width;
	image->height = (int)imageDataResource.height;
	image->component = 4;
	image->bits = 8;
	image->pixel_type = 8;

	image->image.resize(imageDataResource.pixels.size());
	memcpy(image->image.data(), imageDataResource.pixels.data(), imageDataResource.pixels.size());

	return true;
}

static bool FileExists(const std::string &abs_filename, void *user_data)
{
	return HelperFile::exists(abs_filename);
}

static std::string ExpandFilePath(const std::string &filepath, void *user_data)
{
	return filepath;
}

static bool ReadWholeFile(std::vector<unsigned char> *out, std::string *err, const std::string &filepath, void *user_data)
{
	std::string output = "";

	if (!FileIO::open(output, filepath))
	{
		return false;
	}

	if (out)
	{
		out->resize(output.size());
		memcpy(out->data(), output.data(), output.size());
	}

	return true;
}

HelperLoad::HelperLoad(bool convertIndexBuffer) :
	convertIndexBuffer(convertIndexBuffer), model()
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
			if (!model.accessors[i].sparse.isSparse)
			{
				return false;
			}
		}

		accessor.byteOffset = static_cast<uint32_t>(model.accessors[i].byteOffset);

		accessor.normalized = model.accessors[i].normalized;

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

		// Sparse

		if (model.accessors[i].sparse.isSparse)
		{
			accessor.sparse.count = static_cast<uint32_t>(model.accessors[i].sparse.count);

			// Indices

			accessor.sparse.indices.bufferView = model.accessors[i].sparse.indices.bufferView;
			if (accessor.sparse.indices.bufferView >= 0)
			{
				accessor.sparse.indices.pBufferView = &glTF.bufferViews[accessor.sparse.indices.bufferView];
			}
			else
			{
				return false;
			}

			accessor.sparse.indices.byteOffset = static_cast<uint32_t>(model.accessors[i].sparse.indices.byteOffset);

			accessor.sparse.indices.componentType = model.accessors[i].sparse.indices.componentType;
			switch (accessor.sparse.indices.componentType)
			{
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
					accessor.sparse.indices.componentTypeSize = 1;
					accessor.sparse.indices.componentTypeSigned = false;
					accessor.sparse.indices.componentTypeInteger = true;
				break;
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
					accessor.sparse.indices.componentTypeSize = 2;
					accessor.sparse.indices.componentTypeSigned = false;
					accessor.sparse.indices.componentTypeInteger = true;
				break;
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
					accessor.sparse.indices.componentTypeSize = 4;
					accessor.sparse.indices.componentTypeSigned = false;
					accessor.sparse.indices.componentTypeInteger = true;
				break;
			}

			// Values

			accessor.sparse.values.bufferView = model.accessors[i].sparse.values.bufferView;
			if (accessor.sparse.values.bufferView >= 0)
			{
				accessor.sparse.values.pBufferView = &glTF.bufferViews[accessor.sparse.values.bufferView];
			}
			else
			{
				return false;
			}

			accessor.sparse.values.byteOffset = static_cast<uint32_t>(model.accessors[i].sparse.values.byteOffset);

			// Initialize binary data.
			accessor.sparse.buffer.byteLength = accessor.count * accessor.componentTypeSize * accessor.typeCount;
			accessor.sparse.buffer.binary.resize(accessor.sparse.buffer.byteLength);

			accessor.sparse.bufferView.byteOffset = 0;
			accessor.sparse.bufferView.byteLength = accessor.sparse.buffer.byteLength;
			accessor.sparse.bufferView.pBuffer = &accessor.sparse.buffer;

			if (accessor.pBufferView)
			{
				memcpy(accessor.sparse.buffer.binary.data(), HelperAccess::accessData(*accessor.pBufferView) + accessor.byteOffset, accessor.sparse.buffer.byteLength);

				//

				accessor.sparse.bufferView.target = accessor.pBufferView->target;
				accessor.sparse.bufferView.byteStride = accessor.pBufferView->byteStride;
			}

			const uint8_t* indices = HelperAccess::accessData(*accessor.sparse.indices.pBufferView) + accessor.sparse.indices.byteOffset;
			const uint8_t* values = HelperAccess::accessData(*accessor.sparse.values.pBufferView) + accessor.sparse.values.byteOffset;
			for (uint32_t k = 0; k < accessor.sparse.count; k++)
			{
				uint32_t index;

				switch (accessor.sparse.indices.componentTypeSize)
				{
					case 1:
						index = static_cast<uint32_t>(indices[k]);
						break;
					case 2:
						index = static_cast<uint32_t>(*reinterpret_cast<const uint16_t*>(&indices[k * 2]));
						break;
					case 4:
						index = *reinterpret_cast<const uint32_t*>(&indices[k * 4]);
						break;
				}

				//

				uint32_t offsetBinary = index * accessor.componentTypeSize * accessor.typeCount;
				uint32_t offsetValues = k * accessor.componentTypeSize * accessor.typeCount;

				memcpy(&accessor.sparse.buffer.binary.data()[offsetBinary], &values[offsetValues], accessor.componentTypeSize * accessor.typeCount);
			}
		}

		//

		if (convertIndexBuffer && accessor.componentTypeSize == 1)
		{
			const uint8_t* data = reinterpret_cast<const uint8_t*>(HelperAccess::accessData(accessor));

			accessor.aliasedBuffer.byteLength = static_cast<uint32_t>(2 * accessor.count);
			accessor.aliasedBuffer.binary.resize(accessor.aliasedBuffer.byteLength);

			accessor.aliasedBufferView.byteOffset = 0;
			accessor.aliasedBufferView.byteLength = accessor.aliasedBuffer.byteLength;
			accessor.aliasedBufferView.target = 34963;
			accessor.aliasedBufferView.byteStride = 2;
			accessor.aliasedBufferView.pBuffer = &accessor.aliasedBuffer;

			for (uint32_t k = 0; k < accessor.count; k++)
			{
				uint16_t newData = static_cast<uint16_t>(data[k]);
				memcpy(&accessor.aliasedBuffer.binary.data()[k * 2], &newData, sizeof(uint16_t));
			}

			accessor.componentTypeSize = 2;
			accessor.byteOffset = 0;
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

		if (model.images[i].image.size() > 0)
		{
			image.imageDataResources.faceCount = 1;
			image.imageDataResources.mipLevels = 1;
			image.imageDataResources.images.resize(1);

			image.imageDataResources.images[0].width = static_cast<uint32_t>(model.images[i].width);
			image.imageDataResources.images[0].height = static_cast<uint32_t>(model.images[i].height);
			image.imageDataResources.images[0].format = VK_FORMAT_R8G8B8A8_UNORM;
			image.imageDataResources.images[0].pixels.resize(model.images[i].image.size());
			memcpy(image.imageDataResources.images[0].pixels.data(), model.images[i].image.data(), model.images[i].image.size());
		}
		else
		{
			if (!ImageDataIO::open(image.imageDataResources, path + image.uri))
			{
				return false;
			}
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

			//

			if (model.meshes[i].primitives[k].targets.size() > 0)
			{
				primitive.targets.resize(model.meshes[i].primitives[k].targets.size());

				for (uint32_t m = 0; m < primitive.targets.size(); m++)
				{
					const auto& currentTarget = model.meshes[i].primitives[k].targets[m];

					const auto postionIt = currentTarget.find("POSITION");
					if (postionIt != currentTarget.end())
					{
						primitive.targets[m].position = static_cast<int32_t>(postionIt->second);

						if (m == 0)
						{
							primitive.targetPositionData.resize(glTF.accessors[primitive.position].count * primitive.targets.size());
						}

						memcpy(&primitive.targetPositionData.data()[m * glTF.accessors[primitive.position].count], HelperAccess::accessData(glTF.accessors[primitive.position]), sizeof(glm::vec3) * glTF.accessors[primitive.position].count);
					}

					const auto normalIt = currentTarget.find("NORMAL");
					if (normalIt != currentTarget.end())
					{
						primitive.targets[m].normal = static_cast<int32_t>(normalIt->second);

						if (m == 0)
						{
							primitive.targetNormalData.resize(glTF.accessors[primitive.normal].count * primitive.targets.size());
						}

						memcpy(&primitive.targetNormalData.data()[m * glTF.accessors[primitive.normal].count], HelperAccess::accessData(glTF.accessors[primitive.normal]), sizeof(glm::vec3) * glTF.accessors[primitive.normal].count);
					}

					const auto tangentIt = currentTarget.find("TANGENT");
					if (tangentIt != currentTarget.end())
					{
						primitive.targets[m].tangent = static_cast<int32_t>(tangentIt->second);

						if (m == 0)
						{
							primitive.targetTangentData.resize(glTF.accessors[primitive.tangent].count * primitive.targets.size());
						}

						memcpy(&primitive.targetTangentData.data()[m * glTF.accessors[primitive.tangent].count], HelperAccess::accessData(glTF.accessors[primitive.tangent]), sizeof(glm::vec3) * glTF.accessors[primitive.tangent].count);
					}
				}
			}
		}

		//

		if (model.meshes[i].weights.size() > 0)
		{
			mesh.weights.resize(model.meshes[i].weights.size());

			for (size_t k = 0; k < model.meshes[i].weights.size(); k++)
			{
				mesh.weights[k] = static_cast<float>(model.meshes[i].weights[k]);
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

			if (model.nodes[i].weights.size() > 0)
			{
				node.weights.resize(model.nodes[i].weights.size());

				for (size_t k = 0; k < model.nodes[i].weights.size(); k++)
				{
					node.weights[k] = static_cast<float>(model.nodes[i].weights[k]);
				}
			}
		}

		for (size_t k = 0; k < model.nodes[i].children.size(); k++)
		{
			node.children.push_back(model.nodes[i].children[k]);
		}
	}

	return true;
}

bool HelperLoad::initAnimations(GLTF& glTF)
{
	glTF.animations.resize(model.animations.size());

	for (size_t i = 0; i < glTF.animations.size(); i++)
	{
		Animation& animation = glTF.animations[i];

		animation.samplers.resize(model.animations[i].samplers.size());

		for (size_t k = 0; k < animation.samplers.size(); k++)
		{
			AnimationSampler& sampler = animation.samplers[k];

			if (model.animations[i].samplers[k].input >= 0)
			{
				sampler.input = model.animations[i].samplers[k].input;
			}

			if (model.animations[i].samplers[k].output >= 0)
			{
				sampler.output = model.animations[i].samplers[k].output;
			}

			if (model.animations[i].samplers[k].interpolation == "LINEAR")
			{
				sampler.interpolation = LINEAR;
			}
			else if (model.animations[i].samplers[k].interpolation == "STEP")
			{
				sampler.interpolation = STEP;
			}
			else if (model.animations[i].samplers[k].interpolation == "CUBICSPLINE")
			{
				sampler.interpolation = CUBICSPLINE;
			}

			//

			if (sampler.input >= 0 && glTF.accessors[sampler.input].componentType == 5126 && glTF.accessors[sampler.input].typeCount == 1)
			{
				uint32_t range = HelperAccess::getRange(glTF.accessors[sampler.input]);

				sampler.inputTime.resize(range / sizeof(float));
				memcpy(sampler.inputTime.data(), HelperAccess::accessData(glTF.accessors[sampler.input]), range);
			}
			else
			{
				return false;
			}

			if (sampler.output >= 0)
			{
				uint32_t range = HelperAccess::getRange(glTF.accessors[sampler.output]);

				if (glTF.accessors[sampler.output].componentType == 5126)
				{
					sampler.outputValues.resize(range / sizeof(float));
					memcpy(sampler.outputValues.data(), HelperAccess::accessData(glTF.accessors[sampler.output]), range);
				}
				else if (glTF.accessors[sampler.output].componentType == 5123)
				{
					sampler.outputValues.resize(2 * range / sizeof(float));
					const uint16_t* data = reinterpret_cast<const uint16_t*>(HelperAccess::accessData(glTF.accessors[sampler.output]));
					for (size_t m = 0; m < sampler.outputValues.size(); m++)
					{
						sampler.outputValues[m] = data[m] / 65535.0f;
					}
				}
				else if (glTF.accessors[sampler.output].componentType == 5122)
				{
					sampler.outputValues.resize(2 * range / sizeof(float));
					const int16_t* data = reinterpret_cast<const int16_t*>(HelperAccess::accessData(glTF.accessors[sampler.output]));
					for (size_t m = 0; m < sampler.outputValues.size(); m++)
					{
						sampler.outputValues[m] = glm::max(data[m] / 32767.0f, -1.0f);
					}
				}
				else if (glTF.accessors[sampler.output].componentType == 5121)
				{
					sampler.outputValues.resize(4 * range / sizeof(float));
					const uint8_t* data = reinterpret_cast<const uint8_t*>(HelperAccess::accessData(glTF.accessors[sampler.output]));
					for (size_t m = 0; m < sampler.outputValues.size(); m++)
					{
						sampler.outputValues[m] = data[m] / 255.0f;
					}
				}
				else if (glTF.accessors[sampler.output].componentType == 5120)
				{
					sampler.outputValues.resize(4 * range / sizeof(float));
					const int8_t* data = reinterpret_cast<const int8_t*>(HelperAccess::accessData(glTF.accessors[sampler.output]));
					for (size_t m = 0; m < sampler.outputValues.size(); m++)
					{
						sampler.outputValues[m] = glm::max(data[m] / 127.0f, -1.0f);
					}
				}
			}
			else
			{
				return false;
			}
		}

		//

		animation.channels.resize(model.animations[i].channels.size());

		for (size_t k = 0; k < animation.channels.size(); k++)
		{
			AnimationChannel& channel = animation.channels[k];

			if (model.animations[i].channels[k].sampler >= 0)
			{
				channel.sampler = model.animations[i].channels[k].sampler;
			}

			if (model.animations[i].channels[k].target_node >= 0)
			{
				channel.target.node = model.animations[i].channels[k].target_node;
			}

			if (model.animations[i].channels[k].target_path == "translation")
			{
				channel.target.path = translation;
			}
			else if (model.animations[i].channels[k].target_path == "rotation")
			{
				channel.target.path = rotation;
			}
			else if (model.animations[i].channels[k].target_path == "scale")
			{
				channel.target.path = scale;
			}
			else if (model.animations[i].channels[k].target_path == "weights")
			{
				channel.target.path = weights;
			}

			//

			if (channel.sampler >= 0)
			{
				channel.targetSampler = &animation.samplers[channel.sampler];
			}

			if (channel.target.node >= 0)
			{
				channel.target.targetNode = &glTF.nodes[channel.target.node];
			}
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
	std::string output = "";
	if (!FileIO::open(output, filename))
	{
		return false;
	}

	std::string path = HelperFile::getPath(filename);

	// Load glTF

	std::string err = "";
	std::string warn = "";

	tinygltf::FsCallbacks tinygltfCallbacks = {};
	tinygltfCallbacks.FileExists = ::FileExists;
	tinygltfCallbacks.ExpandFilePath = ::ExpandFilePath;
	tinygltfCallbacks.ReadWholeFile = ::ReadWholeFile;

	tinygltf::TinyGLTF tinyGLTF;
	tinyGLTF.SetImageLoader(::LoadImageData, nullptr);
	tinyGLTF.SetFsCallbacks(tinygltfCallbacks);

	bool status = false;
	if (HelperFile::getExtension(filename) == "gltf")
	{
		status = tinyGLTF.LoadASCIIFromString(&model, &err, &warn, output.c_str(), static_cast<uint32_t>(output.length()), path);
	}
	else if (HelperFile::getExtension(filename) == "glb")
	{
		status = tinyGLTF.LoadBinaryFromMemory(&model, &err, &warn, (const unsigned char*)output.c_str(), static_cast<uint32_t>(output.length()), path);
	}

	if (!status)
	{
		return false;
	}

	// Images

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

	// Animations

	if (!initAnimations(glTF))
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
