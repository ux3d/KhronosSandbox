#include "HelperLoader.h"

#include "../../spir/HelperCompiler.h"
#include "../../generic/gltf/HelperAccess.h"

#include "HelperLoop.h"
#include "HelperResource.h"
#include "HelperVulkanAccess.h"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include <tiny_gltf.h>

HelperLoader::HelperLoader(uint32_t width, uint32_t height, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, VkRenderPass renderPass, VkSampleCountFlagBits samples, VkImageView imageView) :
	width(width), height(height), physicalDevice(physicalDevice), device(device), queue(queue), commandPool(commandPool), renderPass(renderPass), samples(samples), imageView(imageView)
{
}

bool HelperLoader::initBuffers(GLTF& glTF)
{
	for (size_t i = 0; i < model.buffers.size(); i++)
	{
		Buffer buffer;

		buffer.uri = model.buffers[i].uri;

		buffer.byteLength = model.buffers[i].data.size();

		buffer.binary.resize(buffer.byteLength);
		memcpy(buffer.binary.data(), model.buffers[i].data.data(), buffer.byteLength);

		glTF.buffers.push_back(buffer);
	}

	return true;
}

bool HelperLoader::initBufferViews(GLTF& glTF, bool useRaytrace)
{
	for (size_t i = 0; i < model.bufferViews.size(); i++)
	{
		BufferView bufferView;

		if (model.bufferViews[i].buffer >= 0)
		{
			bufferView.buffer = model.bufferViews[i].buffer;
			bufferView.pBuffer = &glTF.buffers[bufferView.buffer];
		}
		else
		{
			return false;
		}

		bufferView.byteOffset = model.bufferViews[i].byteOffset;
		bufferView.byteLength = model.bufferViews[i].byteLength;
		bufferView.byteStride = model.bufferViews[i].byteStride;

		bufferView.target = model.bufferViews[i].target;

		//

		if (!HelperResource::initBufferView(bufferView, glTF, physicalDevice, device, queue, commandPool, useRaytrace))
		{
			return false;
		}

		//

		glTF.bufferViews.push_back(bufferView);
	}

	return true;
}

bool HelperLoader::initAccessors(GLTF& glTF)
{

	for (size_t i = 0; i < model.accessors.size(); i++)
	{
		Accessor accessor;

		if (model.accessors[i].bufferView >= 0)
		{
			accessor.bufferView = model.accessors[i].bufferView;
			accessor.pBufferView = &glTF.bufferViews[accessor.bufferView];
		}
		else
		{
			return false;
		}

		accessor.byteOffset = model.accessors[i].byteOffset;

		accessor.count = model.accessors[i].count;

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

		glTF.accessors.push_back(accessor);
	}

	return true;
}

bool HelperLoader::initImages(GLTF& glTF, const std::string& path)
{
	for (size_t i = 0; i < model.images.size(); i++)
	{
		Image image;

		image.uri = model.images[i].uri;
		if (!ImageIO::openImageData(image.imageDataResources, path + image.uri))
		{
			return false;
		}

		glTF.images.push_back(image);
	}

	return true;
}

bool HelperLoader::initSamplers(GLTF& glTF)
{
	for (size_t i = 0; i < model.samplers.size(); i++)
	{
		Sampler sampler;

		switch (model.samplers[i].magFilter)
		{
			case 9728: //NEAREST
				sampler.samplerResourceCreateInfo.magFilter = VK_FILTER_NEAREST;
				break;
			case 9729: //LINEAR
				sampler.samplerResourceCreateInfo.magFilter = VK_FILTER_LINEAR;
				break;
		}

		switch (model.samplers[i].minFilter)
		{
			case 9728: //NEAREST
				sampler.samplerResourceCreateInfo.minFilter = VK_FILTER_NEAREST;
				sampler.samplerResourceCreateInfo.maxLod = 0.25f;	// see Mapping of OpenGL to Vulkan filter modes
				 break;
			case 9729: //LINEAR
				sampler.samplerResourceCreateInfo.minFilter = VK_FILTER_LINEAR;
				sampler.samplerResourceCreateInfo.maxLod = 0.25f;
				break;
			case 9984: //NEAREST_MIPMAP_NEAREST
				sampler.samplerResourceCreateInfo.minFilter = VK_FILTER_NEAREST;
				sampler.samplerResourceCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
				break;
			case 9985: //LINEAR_MIPMAP_NEAREST
				sampler.samplerResourceCreateInfo.minFilter = VK_FILTER_LINEAR;
				sampler.samplerResourceCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
				break;
			case 9986: //NEAREST_MIPMAP_LINEAR
				sampler.samplerResourceCreateInfo.minFilter = VK_FILTER_NEAREST;
				sampler.samplerResourceCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
				break;
			case 9987: //LINEAR_MIPMAP_LINEAR
				sampler.samplerResourceCreateInfo.minFilter = VK_FILTER_LINEAR;
				sampler.samplerResourceCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
				break;
		}

		switch (model.samplers[i].wrapS)
		{
			case 33071:	//CLAMP_TO_EDGE
				sampler.samplerResourceCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
				break;
			case 33648:	//MIRRORED_REPEAT
				sampler.samplerResourceCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
				break;
			case 10497:	//REPEAT
				sampler.samplerResourceCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				break;
		}
		switch (model.samplers[i].wrapT)
		{
			case 33071:	//CLAMP_TO_EDGE
				sampler.samplerResourceCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
				break;
			case 33648:	//MIRRORED_REPEAT
				sampler.samplerResourceCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
				break;
			case 10497:	//REPEAT
				sampler.samplerResourceCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				break;
		}

		glTF.samplers.push_back(sampler);
	}

	return true;
}

bool HelperLoader::initTextures(GLTF& glTF)
{
	for (size_t i = 0; i < model.textures.size(); i++)
	{
		Texture texture;

		TextureResourceCreateInfo textureResourceCreateInfo = {};
		textureResourceCreateInfo.imageDataResources = glTF.images[model.textures[i].source].imageDataResources;
		textureResourceCreateInfo.mipMap = true;

		if (model.textures[i].sampler >= 0)
		{
			textureResourceCreateInfo.samplerResourceCreateInfo = glTF.samplers[model.textures[i].sampler].samplerResourceCreateInfo;
		}

		if (!HelperVulkanResource::createTextureResource(physicalDevice, device, queue, commandPool, texture.textureResource, textureResourceCreateInfo))
		{
			return false;
		}

		glTF.textures.push_back(texture);
	}

	return true;
}

bool HelperLoader::initMaterials(GLTF& glTF)
{
	for (size_t i = 0; i < model.materials.size(); i++)
	{
		Material material;

		uint32_t binding = 0;

		std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

		// Metallic Roughness
		for (const auto& parameterValue : model.materials[i].values)
		{
			if (parameterValue.first == "baseColorTexture")
			{
				material.pbrMetallicRoughness.baseColorTexture.index = parameterValue.second.TextureIndex();
				material.pbrMetallicRoughness.baseColorTexture.texCoord = parameterValue.second.TextureTexCoord();

				//

				VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
				descriptorSetLayoutBinding.binding = binding;
				descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorSetLayoutBinding.descriptorCount = 1;
				descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

				VkDescriptorImageInfo descriptorImageInfo = {};
				descriptorImageInfo.sampler = glTF.textures[material.pbrMetallicRoughness.baseColorTexture.index].textureResource.samplerResource.sampler;
				descriptorImageInfo.imageView = glTF.textures[material.pbrMetallicRoughness.baseColorTexture.index].textureResource.imageViewResource.imageView;
				descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				material.descriptorImageInfos.push_back(descriptorImageInfo);

				//

				material.macros["BASECOLOR_TEXTURE"] = "";
				material.macros["BASECOLOR_BINDING"] = std::to_string(binding);
				material.macros["BASECOLOR_TEXCOORD"] = HelperCompiler::getTexCoord(material.pbrMetallicRoughness.baseColorTexture.texCoord);

				//

				binding++;
			}

			if (parameterValue.first == "baseColorFactor")
			{
				material.pbrMetallicRoughness.baseColorFactor = glm::vec4(parameterValue.second.ColorFactor()[0], parameterValue.second.ColorFactor()[1], parameterValue.second.ColorFactor()[2], parameterValue.second.ColorFactor()[3]);
			}

			if (parameterValue.first == "metallicRoughnessTexture")
			{
				material.pbrMetallicRoughness.metallicRoughnessTexture.index = parameterValue.second.TextureIndex();
				material.pbrMetallicRoughness.metallicRoughnessTexture.texCoord = parameterValue.second.TextureTexCoord();

				//

				VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
				descriptorSetLayoutBinding.binding = binding;
				descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorSetLayoutBinding.descriptorCount = 1;
				descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

				VkDescriptorImageInfo descriptorImageInfo = {};
				descriptorImageInfo.sampler = glTF.textures[material.pbrMetallicRoughness.metallicRoughnessTexture.index].textureResource.samplerResource.sampler;
				descriptorImageInfo.imageView = glTF.textures[material.pbrMetallicRoughness.metallicRoughnessTexture.index].textureResource.imageViewResource.imageView;
				descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				material.descriptorImageInfos.push_back(descriptorImageInfo);

				//

				material.macros["METALLICROUGHNESS_TEXTURE"] = "";
				material.macros["METALLICROUGHNESS_BINDING"] = std::to_string(binding);
				material.macros["METALLICROUGHNESS_TEXCOORD"] = HelperCompiler::getTexCoord(material.pbrMetallicRoughness.metallicRoughnessTexture.texCoord);

				//

				binding++;
			}

			if (parameterValue.first == "metallicFactor")
			{
				material.pbrMetallicRoughness.metallicFactor = parameterValue.second.Factor();
			}

			if (parameterValue.first == "roughnessFactor")
			{
				material.pbrMetallicRoughness.roughnessFactor = parameterValue.second.Factor();
			}
		}

		// Base Material
		for (const auto& parameterValue : model.materials[i].additionalValues)
		{
			if (parameterValue.first == "emissiveTexture")
			{
				material.emissiveTexture.index = parameterValue.second.TextureIndex();
				material.emissiveTexture.texCoord = parameterValue.second.TextureTexCoord();

				//

				VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
				descriptorSetLayoutBinding.binding = binding;
				descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorSetLayoutBinding.descriptorCount = 1;
				descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

				VkDescriptorImageInfo descriptorImageInfo = {};
				descriptorImageInfo.sampler = glTF.textures[material.emissiveTexture.index].textureResource.samplerResource.sampler;
				descriptorImageInfo.imageView = glTF.textures[material.emissiveTexture.index].textureResource.imageViewResource.imageView;
				descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				material.descriptorImageInfos.push_back(descriptorImageInfo);

				//

				material.macros["EMISSIVE_TEXTURE"] = "";
				material.macros["EMISSIVE_BINDING"] = std::to_string(binding);
				material.macros["EMISSIVE_TEXCOORD"] = HelperCompiler::getTexCoord(material.emissiveTexture.texCoord);

				//

				binding++;
			}

			if (parameterValue.first == "emissiveFactor")
			{
				material.emissiveFactor = glm::vec3(parameterValue.second.ColorFactor()[0], parameterValue.second.ColorFactor()[1], parameterValue.second.ColorFactor()[2]);
			}

			if (parameterValue.first == "occlusionTexture")
			{
				material.occlusionTexture.index = parameterValue.second.TextureIndex();
				material.occlusionTexture.texCoord = parameterValue.second.TextureTexCoord();

				//

				VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
				descriptorSetLayoutBinding.binding = binding;
				descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorSetLayoutBinding.descriptorCount = 1;
				descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

				VkDescriptorImageInfo descriptorImageInfo = {};
				descriptorImageInfo.sampler = glTF.textures[material.occlusionTexture.index].textureResource.samplerResource.sampler;
				descriptorImageInfo.imageView = glTF.textures[material.occlusionTexture.index].textureResource.imageViewResource.imageView;
				descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				material.descriptorImageInfos.push_back(descriptorImageInfo);

				//

				material.macros["OCCLUSION_TEXTURE"] = "";
				material.macros["OCCLUSION_BINDING"] = std::to_string(binding);
				material.macros["OCCLUSION_TEXCOORD"] = HelperCompiler::getTexCoord(material.occlusionTexture.texCoord);

				//

				binding++;

				//
				//

				material.occlusionTexture.strength = parameterValue.second.TextureStrength();
			}

			if (parameterValue.first == "normalTexture")
			{
				material.normalTexture.index = parameterValue.second.TextureIndex();
				material.normalTexture.texCoord = parameterValue.second.TextureTexCoord();

				//

				VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
				descriptorSetLayoutBinding.binding = binding;
				descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorSetLayoutBinding.descriptorCount = 1;
				descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

				VkDescriptorImageInfo descriptorImageInfo = {};
				descriptorImageInfo.sampler = glTF.textures[material.normalTexture.index].textureResource.samplerResource.sampler;
				descriptorImageInfo.imageView = glTF.textures[material.normalTexture.index].textureResource.imageViewResource.imageView;
				descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				material.descriptorImageInfos.push_back(descriptorImageInfo);

				//

				material.macros["NORMAL_TEXTURE"] = "";
				material.macros["NORMAL_BINDING"] = std::to_string(binding);
				material.macros["NORMAL_TEXCOORD"] = HelperCompiler::getTexCoord(material.normalTexture.texCoord);

				//

				binding++;

				//
				//

				material.normalTexture.scale = parameterValue.second.TextureScale();
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

		material.alphaCutoff = model.materials[i].alphaCutoff;

		material.doubleSided = model.materials[i].doubleSided;

		//
		//

		VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
		descriptorSetLayoutBinding.binding = binding;
		descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding.descriptorCount = 1;
		descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

		VkDescriptorImageInfo descriptorImageInfo = {};
		descriptorImageInfo.sampler = glTF.diffuse.samplerResource.sampler;
		descriptorImageInfo.imageView = glTF.diffuse.imageViewResource.imageView;
		descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		material.descriptorImageInfos.push_back(descriptorImageInfo);

		material.macros["DIFFUSE_BINDING"] = std::to_string(binding);

		binding++;

		//

		descriptorSetLayoutBinding = {};
		descriptorSetLayoutBinding.binding = binding;
		descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding.descriptorCount = 1;
		descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

		descriptorImageInfo = {};
		descriptorImageInfo.sampler = glTF.specular.samplerResource.sampler;
		descriptorImageInfo.imageView = glTF.specular.imageViewResource.imageView;
		descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		material.descriptorImageInfos.push_back(descriptorImageInfo);

		material.macros["SPECULAR_BINDING"] = std::to_string(binding);

		binding++;

		//

		descriptorSetLayoutBinding = {};
		descriptorSetLayoutBinding.binding = binding;
		descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding.descriptorCount = 1;
		descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

		descriptorImageInfo = {};
		descriptorImageInfo.sampler = glTF.lut.samplerResource.sampler;
		descriptorImageInfo.imageView = glTF.lut.imageViewResource.imageView;
		descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		material.descriptorImageInfos.push_back(descriptorImageInfo);

		material.macros["LUT_BINDING"] = std::to_string(binding);

		binding++;

		//
		//

		UniformBufferResourceCreateInfo uniformBufferResourceCreateInfo = {};

		uniformBufferResourceCreateInfo.bufferResourceCreateInfo.size = sizeof(MaterialUniformBuffer);
		uniformBufferResourceCreateInfo.bufferResourceCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

		if (!HelperVulkanResource::createUniformBufferResource(physicalDevice, device, material.uniformBufferResource, uniformBufferResourceCreateInfo))
		{
			return false;
		}

		descriptorSetLayoutBinding = {};
		descriptorSetLayoutBinding.binding = binding;
		descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorSetLayoutBinding.descriptorCount = 1;
		descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

		material.descriptorBufferInfo.buffer = material.uniformBufferResource.bufferResource.buffer;
		material.descriptorBufferInfo.offset = 0;
		material.descriptorBufferInfo.range = sizeof(MaterialUniformBuffer);

		material.macros["UNIFORMBUFFER_BINDING"] = std::to_string(binding);

		binding++;

		//

		if (!HelperResource::initMaterial(material, glTF, physicalDevice, device, descriptorSetLayoutBindings))
		{
			return false;
		}

		//

		MaterialUniformBuffer uniformBuffer = {};

		uniformBuffer.baseColorFactor = material.pbrMetallicRoughness.baseColorFactor;

		uniformBuffer.metallicFactor = material.pbrMetallicRoughness.metallicFactor;
		uniformBuffer.roughnessFactor = material.pbrMetallicRoughness.roughnessFactor;

		uniformBuffer.normalScale = material.normalTexture.scale;
		uniformBuffer.occlusionStrength = material.occlusionTexture.strength;

		uniformBuffer.emissiveFactor = material.emissiveFactor;

		uniformBuffer.alphaMode = material.alphaMode;
		uniformBuffer.alphaCutoff = material.alphaCutoff;

		uniformBuffer.doubleSided = material.doubleSided;

		if (!HelperVulkanResource::copyHostToDevice(device, material.uniformBufferResource.bufferResource, &uniformBuffer, sizeof(uniformBuffer)))
		{
			return false;
		}

		//

		glTF.materials.push_back(material);
	}

	return true;
}

bool HelperLoader::initMeshes(GLTF& glTF, bool useRaytrace)
{
	//
	// Load the shader code.
	//

	std::string vertexShaderSource = "";
	if (!FileIO::openText(vertexShaderSource, "../Resources/shaders/gltf.vert"))
	{
		return false;
	}

	std::string fragmentShaderSource = "";
	if (!FileIO::openText(fragmentShaderSource, "../Resources/shaders/gltf.frag"))
	{
		return false;
	}

	for (size_t i = 0; i < model.meshes.size(); i++)
	{
		Mesh mesh;

		for (size_t k = 0; k < model.meshes[i].primitives.size(); k++)
		{
			Primitive primitive;

			std::map<std::string, std::string> macros;

			if (model.meshes[i].primitives[k].indices >= 0)
			{
				primitive.indices = model.meshes[i].primitives[k].indices;
			}

			VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
			if (model.meshes[i].primitives[k].material >= 0)
			{
				primitive.material = model.meshes[i].primitives[k].material;

				macros = glTF.materials[primitive.material].macros;

				if (glTF.materials[primitive.material].doubleSided)
				{
					cullMode = VK_CULL_MODE_NONE;
				}
			}

			uint32_t attributesCount = model.meshes[i].primitives[k].attributes.size();
			primitive.vertexBuffers.resize(attributesCount);
			primitive.vertexBuffersOffsets.resize(attributesCount);

			uint32_t attributesIndex = 0;
			primitive.vertexInputBindingDescriptions.resize(attributesCount);
			primitive.vertexInputAttributeDescriptions.resize(attributesCount);

			uint32_t location = 0;
			uint32_t binding = 0;

			for (const auto& attribute : model.meshes[i].primitives[k].attributes)
			{
				uint32_t accessorIndex = attribute.second;

				uint32_t stride = glTF.accessors[accessorIndex].typeCount * glTF.accessors[accessorIndex].componentTypeSize;

				VkFormat format = VK_FORMAT_UNDEFINED;

				if (!HelperVulkan::getFormat(format, glTF.accessors[accessorIndex].componentTypeSize, glTF.accessors[accessorIndex].componentTypeSigned, glTF.accessors[accessorIndex].componentTypeInteger, glTF.accessors[accessorIndex].typeCount))
				{
					return false;
				}

				if (attribute.first == "POSITION")
				{
					primitive.position = accessorIndex;
					primitive.positionLocation = attributesIndex;

					if (glTF.accessors[primitive.position].typeCount == 3)
					{
						macros["POSITION_VEC3"] = "";
					}
					else if (glTF.accessors[primitive.position].typeCount == 4)
					{
						macros["POSITION_VEC4"] = "";
					}
					else
					{
						return false;
					}

					macros["POSITION_LOC"] = std::to_string(location);
				}
				else if (attribute.first == "NORMAL")
				{
					primitive.normal = accessorIndex;
					primitive.normalLocation = attributesIndex;

					if (glTF.accessors[primitive.normal].typeCount == 3)
					{
						macros["NORMAL_VEC3"] = "";
					}
					else
					{
						return false;
					}

					macros["NORMAL_LOC"] = std::to_string(location);
				}
				else if (attribute.first == "TANGENT")
				{
					primitive.tangent = accessorIndex;
					primitive.tangentLocation = attributesIndex;

					if (glTF.accessors[primitive.tangent].typeCount == 4)
					{
						macros["TANGENT_VEC4"] = "";
					}
					else
					{
						return false;
					}

					macros["TANGENT_LOC"] = std::to_string(location);
				}
				else if (attribute.first == "TEXCOORD_0")
				{
					primitive.texCoord0 = accessorIndex;
					primitive.texCoord0Location = attributesIndex;

					if (glTF.accessors[primitive.texCoord0].typeCount == 2)
					{
						macros["TEXCOORD_0_VEC2"] = "";
					}
					else
					{
						return false;
					}

					macros["TEXCOORD_0_LOC"] = std::to_string(location);
				}
				else if (attribute.first == "TEXCOORD_1")
				{
					primitive.texCoord1 = accessorIndex;
					primitive.texCoord1Location = attributesIndex;

					if (glTF.accessors[primitive.texCoord1].typeCount == 2)
					{
						macros["TEXCOORD_1_VEC2"] = "";
					}
					else
					{
						return false;
					}

					macros["TEXCOORD_1_LOC"] = std::to_string(location);
				}
				else if (attribute.first == "COLOR_0")
				{
					primitive.color0 = accessorIndex;
					primitive.color0Location = attributesIndex;

					if (glTF.accessors[primitive.color0].typeCount == 4)
					{
						macros["COLOR_0_VEC4"] = "";
					}
					else if (glTF.accessors[primitive.color0].typeCount == 3)
					{
						macros["COLOR_0_VEC3"] = "";
					}
					else
					{
						return false;
					}

					macros["COLOR_0_LOC"] = std::to_string(location);
				}
				else if (attribute.first == "JOINTS_0")
				{
					primitive.joints0 = accessorIndex;
					primitive.joints0Location = attributesIndex;

					if (glTF.accessors[primitive.joints0].typeCount == 4)
					{
						macros["JOINTS_0_VEC4"] = "";
					}
					else
					{
						return false;
					}

					macros["JOINTS_0_LOC"] = std::to_string(location);
				}
				else if (attribute.first == "JOINTS_1")
				{
					primitive.joints1 = accessorIndex;
					primitive.joints1Location = attributesIndex;

					if (glTF.accessors[primitive.joints1].typeCount == 4)
					{
						macros["JOINTS_1_VEC4"] = "";
					}
					else
					{
						return false;
					}

					macros["JOINTS_1_LOC"] = std::to_string(location);
				}
				else if (attribute.first == "WEIGHTS_0")
				{
					primitive.weights0 = accessorIndex;
					primitive.weights0Location = attributesIndex;

					if (glTF.accessors[primitive.weights0].typeCount == 4)
					{
						macros["WEIGHTS_0_VEC4"] = "";
					}
					else
					{
						return false;
					}

					macros["WEIGHTS_0_LOC"] = std::to_string(location);
				}
				else if (attribute.first == "WEIGHTS_1")
				{
					primitive.weights1 = accessorIndex;
					primitive.weights1Location = attributesIndex;

					if (glTF.accessors[primitive.weights1].typeCount == 4)
					{
						macros["WEIGHTS_1_VEC4"] = "";
					}
					else
					{
						return false;
					}

					macros["WEIGHTS_1_LOC"] = std::to_string(location);
				}
				else
				{
					// Not supported.

					continue;
				}

				//

				primitive.vertexInputBindingDescriptions[attributesIndex].binding = binding;
				primitive.vertexInputBindingDescriptions[attributesIndex].stride = stride;
				primitive.vertexInputBindingDescriptions[attributesIndex].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				primitive.vertexInputAttributeDescriptions[attributesIndex].binding = binding;
				primitive.vertexInputAttributeDescriptions[attributesIndex].location = location;
				primitive.vertexInputAttributeDescriptions[attributesIndex].format = format;
				primitive.vertexInputAttributeDescriptions[attributesIndex].offset = 0;

				//

				primitive.vertexBuffers[attributesIndex] = HelperVulkanAccess::getBuffer(glTF.accessors[accessorIndex]);
				primitive.vertexBuffersOffsets[attributesIndex] = HelperAccess::getOffset(glTF.accessors[accessorIndex]);

				//

				attributesIndex++;
				location++;
				binding++;
			}

			primitive.attributesCount = attributesIndex;

			if (primitive.position < 0)
			{
				return false;
			}

			//

			std::vector<uint32_t> vertexShaderCode;
			if (!Compiler::buildShader(vertexShaderCode, vertexShaderSource, macros, shaderc_vertex_shader))
			{
				return false;
			}

			std::vector<uint32_t> fragmentShaderCode;
			if (!Compiler::buildShader(fragmentShaderCode, fragmentShaderSource, macros, shaderc_fragment_shader))
			{
				return false;
			}

			if (!HelperVulkanResource::createShaderModule(primitive.vertexShaderModule, device, vertexShaderCode))
			{
				return false;
			}

			if (!HelperVulkanResource::createShaderModule(primitive.fragmentShaderModule, device, fragmentShaderCode))
			{
				return false;
			}

			if (!HelperResource::initPrimitive(primitive, glTF, physicalDevice, device, queue, commandPool, width, height, renderPass, samples, &glTF.materials[primitive.material].descriptorSetLayout, cullMode, useRaytrace))
			{
				return false;
			}

			//

			mesh.primitives.push_back(primitive);
		}

		glTF.meshes.push_back(mesh);
	}

	return true;
}

bool HelperLoader::initNodes(GLTF& glTF, bool useRaytrace)
{
	for (size_t i = 0; i < model.nodes.size(); i++)
	{
		Node node;

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
			node.rotation = glm::quat(model.nodes[i].rotation[3], model.nodes[i].rotation[0], model.nodes[i].rotation[1], model.nodes[i].rotation[2]);
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

		glTF.nodes.push_back(node);
	}

	return true;
}

bool HelperLoader::initScenes(GLTF& glTF, bool useRaytrace)
{
	for (size_t i = 0; i < model.scenes.size(); i++)
	{
		Scene scene;

		for (size_t k = 0; k < model.scenes[i].nodes.size(); k++)
		{
			scene.nodes.push_back(model.scenes[i].nodes[k]);
		}

		glTF.scenes.push_back(scene);
	}

	if (!HelperLoop::update(glTF, glm::mat4(1.0f)))
	{
		return false;
	}

	for (size_t i = 0; i < glTF.scenes.size(); i++)
	{
		if (!HelperResource::initScene(glTF.scenes[i], glTF, physicalDevice, device, queue, commandPool, imageView, useRaytrace))
		{
			return false;
		}
	}

	return true;
}

bool HelperLoader::open(GLTF& glTF, const std::string& filename, const std::string& environment, bool useRaytrace)
{
	// Diffuse

	std::string diffuseFilename = environment + "/" + "diffuse.ktx2";
	TextureResourceCreateInfo diffuseMap = {};
	diffuseMap.samplerResourceCreateInfo.minFilter = VK_FILTER_LINEAR;
	diffuseMap.samplerResourceCreateInfo.magFilter = VK_FILTER_LINEAR;
	diffuseMap.samplerResourceCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;

	if(!ImageIO::openImageData(diffuseMap.imageDataResources, diffuseFilename))
	{
		return false;
	}

	if (!HelperVulkanResource::createTextureResource(physicalDevice, device, queue, commandPool, glTF.diffuse, diffuseMap))
	{
		return false;
	}

	// Specular

	std::string specularFilename = environment + "/" + "specular.ktx2";
	TextureResourceCreateInfo specularMap = {};
	specularMap.samplerResourceCreateInfo.minFilter = VK_FILTER_LINEAR;
	specularMap.samplerResourceCreateInfo.magFilter = VK_FILTER_LINEAR;
	specularMap.samplerResourceCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

	if(!ImageIO::openImageData(specularMap.imageDataResources, specularFilename))
	{
		return false;
	}

	if (!HelperVulkanResource::createTextureResource(physicalDevice, device, queue, commandPool, glTF.specular, specularMap))
	{
		return false;
	}

	// LUT

	std::string lutFilename = "../Resources/brdf/lut_ggx.png";
	TextureResourceCreateInfo lutMap = {};
	lutMap.samplerResourceCreateInfo.minFilter = VK_FILTER_LINEAR;
	lutMap.samplerResourceCreateInfo.magFilter = VK_FILTER_LINEAR;
	lutMap.samplerResourceCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	lutMap.samplerResourceCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	lutMap.samplerResourceCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

	if(!ImageIO::openImageData(lutMap.imageDataResources, lutFilename))
	{
		return false;
	}

	if (!HelperVulkanResource::createTextureResource(physicalDevice, device, queue, commandPool, glTF.lut, lutMap))
	{
		return false;
	}

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

	std::string path = FileIO::getPath(filename);
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

	if (!initBufferViews(glTF, useRaytrace))
	{
		return false;
	}

	// Accessors

	if (!initAccessors(glTF))
	{
		return false;
	}

	// Meshes

	if (!initMeshes(glTF, useRaytrace))
	{
		return false;
	}

	// Nodes

	if (!initNodes(glTF, useRaytrace))
	{
		return false;
	}

	// Scenes

	if (!initScenes(glTF, useRaytrace))
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
