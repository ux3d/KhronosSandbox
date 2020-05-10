#include "HelperLoader.h"

#include "../../shader/HelperShader.h"
#include "../HelperAccess.h"
#include "HelperAccessResource.h"
#include "HelperLoop.h"
#include "ResourceManager.h"

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
	width(width), height(height), physicalDevice(physicalDevice), device(device), queue(queue), commandPool(commandPool), renderPass(renderPass), samples(samples), imageView(imageView), model()
{
}

bool HelperLoader::initBuffers(ResourceManager& resourceManager, GLTF& glTF)
{
	glTF.buffers.resize(model.buffers.size());

	for (size_t i = 0; i < model.buffers.size(); i++)
	{
		Buffer& buffer = glTF.buffers[i];

		buffer.uri = model.buffers[i].uri;

		buffer.byteLength = model.buffers[i].data.size();

		buffer.binary.resize(buffer.byteLength);
		memcpy(buffer.binary.data(), model.buffers[i].data.data(), buffer.byteLength);
	}

	return true;
}

bool HelperLoader::initBufferViews(ResourceManager& resourceManager, GLTF& glTF, bool useRaytrace)
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

		bufferView.byteOffset = model.bufferViews[i].byteOffset;
		bufferView.byteLength = model.bufferViews[i].byteLength;
		bufferView.byteStride = model.bufferViews[i].byteStride;

		bufferView.target = model.bufferViews[i].target;

		//

		if (!resourceManager.initBufferView(bufferView, glTF, physicalDevice, device, queue, commandPool, useRaytrace))
		{
			return false;
		}
	}

	return true;
}

bool HelperLoader::initAccessors(ResourceManager& resourceManager, GLTF& glTF)
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
	}

	return true;
}

bool HelperLoader::initImages(ResourceManager& resourceManager, GLTF& glTF, const std::string& path)
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

bool HelperLoader::initSamplers(ResourceManager& resourceManager, GLTF& glTF)
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

bool HelperLoader::initTextures(ResourceManager& resourceManager, GLTF& glTF)
{
	glTF.textures.resize(model.textures.size());

	for (size_t i = 0; i < glTF.textures.size(); i++)
	{
		Texture& texture = glTF.textures[i];

		texture.source = model.textures[i].source;
		texture.sampler = model.textures[i].sampler;

		//

		TextureResourceCreateInfo textureResourceCreateInfo = {};
		textureResourceCreateInfo.imageDataResources = glTF.images[texture.source].imageDataResources;
		textureResourceCreateInfo.mipMap = true;

		if (texture.sampler >= 0)
		{
			textureResourceCreateInfo.samplerResourceCreateInfo.magFilter = glTF.samplers[texture.sampler].magFilter;
			textureResourceCreateInfo.samplerResourceCreateInfo.minFilter = glTF.samplers[texture.sampler].minFilter;
			textureResourceCreateInfo.samplerResourceCreateInfo.mipmapMode = glTF.samplers[texture.sampler].mipmapMode;
			textureResourceCreateInfo.samplerResourceCreateInfo.addressModeU = glTF.samplers[texture.sampler].addressModeU;
			textureResourceCreateInfo.samplerResourceCreateInfo.addressModeV = glTF.samplers[texture.sampler].addressModeV;
			textureResourceCreateInfo.samplerResourceCreateInfo.minLod = glTF.samplers[texture.sampler].minLod;
			textureResourceCreateInfo.samplerResourceCreateInfo.maxLod = glTF.samplers[texture.sampler].maxLod;
		}

		if (!VulkanResource::createTextureResource(physicalDevice, device, queue, commandPool, *resourceManager.getTextureResource(&texture), textureResourceCreateInfo))
		{
			return false;
		}
	}

	return true;
}

bool HelperLoader::initMaterials(ResourceManager& resourceManager, GLTF& glTF)
{
	glTF.materials.resize(model.materials.size());

	for (size_t i = 0; i < glTF.materials.size(); i++)
	{
		Material& material = glTF.materials[i];
		MaterialResource* materialResource = resourceManager.getMaterialResource(&material);

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

				TextureResource* textureResource = resourceManager.getTextureResource(&glTF.textures[material.pbrMetallicRoughness.baseColorTexture.index]);

				//

				VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
				descriptorSetLayoutBinding.binding = binding;
				descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorSetLayoutBinding.descriptorCount = 1;
				descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

				VkDescriptorImageInfo descriptorImageInfo = {};
				descriptorImageInfo.sampler = textureResource->samplerResource.sampler;
				descriptorImageInfo.imageView = textureResource->imageViewResource.imageView;
				descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				materialResource->descriptorImageInfos.push_back(descriptorImageInfo);

				//

				materialResource->macros["BASECOLOR_TEXTURE"] = "";
				materialResource->macros["BASECOLOR_BINDING"] = std::to_string(binding);
				materialResource->macros["BASECOLOR_TEXCOORD"] = HelperShader::getTexCoord(material.pbrMetallicRoughness.baseColorTexture.texCoord);

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

				TextureResource* textureResource = resourceManager.getTextureResource(&glTF.textures[material.pbrMetallicRoughness.metallicRoughnessTexture.index]);

				//

				VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
				descriptorSetLayoutBinding.binding = binding;
				descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorSetLayoutBinding.descriptorCount = 1;
				descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

				VkDescriptorImageInfo descriptorImageInfo = {};
				descriptorImageInfo.sampler = textureResource->samplerResource.sampler;
				descriptorImageInfo.imageView = textureResource->imageViewResource.imageView;
				descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				materialResource->descriptorImageInfos.push_back(descriptorImageInfo);

				//

				materialResource->macros["METALLICROUGHNESS_TEXTURE"] = "";
				materialResource->macros["METALLICROUGHNESS_BINDING"] = std::to_string(binding);
				materialResource->macros["METALLICROUGHNESS_TEXCOORD"] = HelperShader::getTexCoord(material.pbrMetallicRoughness.metallicRoughnessTexture.texCoord);

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

				TextureResource* textureResource = resourceManager.getTextureResource(&glTF.textures[material.emissiveTexture.index]);

				//

				VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
				descriptorSetLayoutBinding.binding = binding;
				descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorSetLayoutBinding.descriptorCount = 1;
				descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

				VkDescriptorImageInfo descriptorImageInfo = {};
				descriptorImageInfo.sampler = textureResource->samplerResource.sampler;
				descriptorImageInfo.imageView = textureResource->imageViewResource.imageView;
				descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				materialResource->descriptorImageInfos.push_back(descriptorImageInfo);

				//

				materialResource->macros["EMISSIVE_TEXTURE"] = "";
				materialResource->macros["EMISSIVE_BINDING"] = std::to_string(binding);
				materialResource->macros["EMISSIVE_TEXCOORD"] = HelperShader::getTexCoord(material.emissiveTexture.texCoord);

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

				TextureResource* textureResource = resourceManager.getTextureResource(&glTF.textures[material.occlusionTexture.index]);

				//

				VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
				descriptorSetLayoutBinding.binding = binding;
				descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorSetLayoutBinding.descriptorCount = 1;
				descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

				VkDescriptorImageInfo descriptorImageInfo = {};
				descriptorImageInfo.sampler = textureResource->samplerResource.sampler;
				descriptorImageInfo.imageView = textureResource->imageViewResource.imageView;
				descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				materialResource->descriptorImageInfos.push_back(descriptorImageInfo);

				//

				materialResource->macros["OCCLUSION_TEXTURE"] = "";
				materialResource->macros["OCCLUSION_BINDING"] = std::to_string(binding);
				materialResource->macros["OCCLUSION_TEXCOORD"] = HelperShader::getTexCoord(material.occlusionTexture.texCoord);

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

				TextureResource* textureResource = resourceManager.getTextureResource(&glTF.textures[material.normalTexture.index]);

				//

				VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
				descriptorSetLayoutBinding.binding = binding;
				descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorSetLayoutBinding.descriptorCount = 1;
				descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

				VkDescriptorImageInfo descriptorImageInfo = {};
				descriptorImageInfo.sampler = textureResource->samplerResource.sampler;
				descriptorImageInfo.imageView = textureResource->imageViewResource.imageView;
				descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				materialResource->descriptorImageInfos.push_back(descriptorImageInfo);

				//

				materialResource->macros["NORMAL_TEXTURE"] = "";
				materialResource->macros["NORMAL_BINDING"] = std::to_string(binding);
				materialResource->macros["NORMAL_TEXCOORD"] = HelperShader::getTexCoord(material.normalTexture.texCoord);

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

		GltfResource* gltfResource = resourceManager.getGltfResource();

		//
		//

		VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
		descriptorSetLayoutBinding.binding = binding;
		descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding.descriptorCount = 1;
		descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

		VkDescriptorImageInfo descriptorImageInfo = {};
		descriptorImageInfo.sampler = gltfResource->diffuse.samplerResource.sampler;
		descriptorImageInfo.imageView = gltfResource->diffuse.imageViewResource.imageView;
		descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		materialResource->descriptorImageInfos.push_back(descriptorImageInfo);

		materialResource->macros["DIFFUSE_BINDING"] = std::to_string(binding);

		binding++;

		//

		descriptorSetLayoutBinding = {};
		descriptorSetLayoutBinding.binding = binding;
		descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding.descriptorCount = 1;
		descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

		descriptorImageInfo = {};
		descriptorImageInfo.sampler = gltfResource->specular.samplerResource.sampler;
		descriptorImageInfo.imageView = gltfResource->specular.imageViewResource.imageView;
		descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		materialResource->descriptorImageInfos.push_back(descriptorImageInfo);

		materialResource->macros["SPECULAR_BINDING"] = std::to_string(binding);

		binding++;

		//

		descriptorSetLayoutBinding = {};
		descriptorSetLayoutBinding.binding = binding;
		descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding.descriptorCount = 1;
		descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

		descriptorImageInfo = {};
		descriptorImageInfo.sampler = gltfResource->lut.samplerResource.sampler;
		descriptorImageInfo.imageView = gltfResource->lut.imageViewResource.imageView;
		descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		materialResource->descriptorImageInfos.push_back(descriptorImageInfo);

		materialResource->macros["LUT_BINDING"] = std::to_string(binding);

		binding++;

		//
		//

		UniformBufferResourceCreateInfo uniformBufferResourceCreateInfo = {};

		uniformBufferResourceCreateInfo.bufferResourceCreateInfo.size = sizeof(MaterialUniformBuffer);
		uniformBufferResourceCreateInfo.bufferResourceCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

		if (!VulkanResource::createUniformBufferResource(physicalDevice, device, materialResource->uniformBufferResource, uniformBufferResourceCreateInfo))
		{
			return false;
		}

		descriptorSetLayoutBinding = {};
		descriptorSetLayoutBinding.binding = binding;
		descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorSetLayoutBinding.descriptorCount = 1;
		descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

		materialResource->descriptorBufferInfo.buffer = materialResource->uniformBufferResource.bufferResource.buffer;
		materialResource->descriptorBufferInfo.offset = 0;
		materialResource->descriptorBufferInfo.range = sizeof(MaterialUniformBuffer);

		materialResource->macros["UNIFORMBUFFER_BINDING"] = std::to_string(binding);

		binding++;

		//

		if (!resourceManager.initMaterial(glTF.materials.back(), glTF, physicalDevice, device, descriptorSetLayoutBindings))
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

		if (!VulkanResource::copyHostToDevice(device, materialResource->uniformBufferResource.bufferResource, &uniformBuffer, sizeof(uniformBuffer)))
		{
			return false;
		}
	}

	return true;
}

bool HelperLoader::initMeshes(ResourceManager& resourceManager, GLTF& glTF, bool useRaytrace)
{
	//
	// Load the shader code.
	//

	std::string vertexShaderSource = "";
	if (!FileIO::open(vertexShaderSource, "../Resources/shaders/gltf.vert"))
	{
		return false;
	}

	std::string fragmentShaderSource = "";
	if (!FileIO::open(fragmentShaderSource, "../Resources/shaders/gltf.frag"))
	{
		return false;
	}

	glTF.meshes.resize(model.meshes.size());

	for (size_t i = 0; i < glTF.meshes.size(); i++)
	{
		Mesh& mesh = glTF.meshes[i];

		mesh.primitives.resize(model.meshes[i].primitives.size());

		for (size_t k = 0; k < mesh.primitives.size(); k++)
		{
			Primitive& primitive = mesh.primitives[k];

			PrimitiveResource* primitiveResource = resourceManager.getPrimitiveResource(&primitive);

			std::map<std::string, std::string> macros;

			if (model.meshes[i].primitives[k].indices >= 0)
			{
				primitive.indices = model.meshes[i].primitives[k].indices;
			}

			VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
			if (model.meshes[i].primitives[k].material >= 0)
			{
				primitive.material = model.meshes[i].primitives[k].material;

				macros = resourceManager.getMaterialResource(&glTF.materials[primitive.material])->macros;

				if (glTF.materials[primitive.material].doubleSided)
				{
					cullMode = VK_CULL_MODE_NONE;
				}
			}

			uint32_t attributesCount = model.meshes[i].primitives[k].attributes.size();
			primitiveResource->vertexBuffers.resize(attributesCount);
			primitiveResource->vertexBuffersOffsets.resize(attributesCount);

			uint32_t attributeIndex = 0;
			primitiveResource->vertexInputBindingDescriptions.resize(attributesCount);
			primitiveResource->vertexInputAttributeDescriptions.resize(attributesCount);

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
					primitiveResource->positionAttributeIndex = attributeIndex;

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
					primitiveResource->normalAttributeIndex = attributeIndex;

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
					primitiveResource->tangentAttributeIndex = attributeIndex;

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
					primitiveResource->texCoord0AttributeIndex = attributeIndex;

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
					primitiveResource->texCoord1AttributeIndex = attributeIndex;

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
					primitiveResource->color0AttributeIndex = attributeIndex;

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
					primitiveResource->joints0AttributeIndex = attributeIndex;

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
					primitiveResource->joints1AttributeIndex = attributeIndex;

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
					primitiveResource->weights0AttributeIndex = attributeIndex;

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
					primitiveResource->weights1AttributeIndex = attributeIndex;

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

				primitiveResource->vertexInputBindingDescriptions[attributeIndex].binding = binding;
				primitiveResource->vertexInputBindingDescriptions[attributeIndex].stride = stride;
				primitiveResource->vertexInputBindingDescriptions[attributeIndex].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].binding = binding;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].location = location;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].format = format;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].offset = 0;

				//

				primitiveResource->vertexBuffers[attributeIndex] = HelperAccessResource::getBuffer(resourceManager, &glTF.accessors[accessorIndex]);
				primitiveResource->vertexBuffersOffsets[attributeIndex] = HelperAccess::getOffset(glTF.accessors[accessorIndex]);

				//

				attributeIndex++;
				location++;
				binding++;
			}

			primitiveResource->attributesCount = attributeIndex;

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

			if (!VulkanResource::createShaderModule(primitiveResource->vertexShaderModule, device, vertexShaderCode))
			{
				return false;
			}

			if (!VulkanResource::createShaderModule(primitiveResource->fragmentShaderModule, device, fragmentShaderCode))
			{
				return false;
			}

			if (!resourceManager.initPrimitive(primitive, glTF, physicalDevice, device, queue, commandPool, width, height, renderPass, samples, &resourceManager.getMaterialResource(&glTF.materials[primitive.material])->descriptorSetLayout, cullMode, useRaytrace))
			{
				return false;
			}
		}
	}

	return true;
}

bool HelperLoader::initNodes(ResourceManager& resourceManager, GLTF& glTF, bool useRaytrace)
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
	}

	return true;
}

bool HelperLoader::initScenes(ResourceManager& resourceManager, GLTF& glTF, bool useRaytrace)
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

	if (!HelperLoop::update(resourceManager, glTF, glm::mat4(1.0f)))
	{
		return false;
	}

	for (size_t i = 0; i < glTF.scenes.size(); i++)
	{
		if (!resourceManager.initScene(glTF.scenes[i], glTF, physicalDevice, device, queue, commandPool, imageView, useRaytrace))
		{
			return false;
		}
	}

	return true;
}

bool HelperLoader::open(ResourceManager& resourceManager, GLTF& glTF, const std::string& filename, const std::string& environment, bool useRaytrace)
{
	GltfResource* gltfResource = resourceManager.getGltfResource();

	// Diffuse

	std::string diffuseFilename = environment + "/" + "diffuse.ktx2";
	TextureResourceCreateInfo diffuseMap = {};
	diffuseMap.samplerResourceCreateInfo.minFilter = VK_FILTER_LINEAR;
	diffuseMap.samplerResourceCreateInfo.magFilter = VK_FILTER_LINEAR;
	diffuseMap.samplerResourceCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;

	if(!ImageDataIO::open(diffuseMap.imageDataResources, diffuseFilename))
	{
		return false;
	}

	if (!VulkanResource::createTextureResource(physicalDevice, device, queue, commandPool, gltfResource->diffuse, diffuseMap))
	{
		return false;
	}

	// Specular

	std::string specularFilename = environment + "/" + "specular.ktx2";
	TextureResourceCreateInfo specularMap = {};
	specularMap.samplerResourceCreateInfo.minFilter = VK_FILTER_LINEAR;
	specularMap.samplerResourceCreateInfo.magFilter = VK_FILTER_LINEAR;
	specularMap.samplerResourceCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

	if(!ImageDataIO::open(specularMap.imageDataResources, specularFilename))
	{
		return false;
	}

	if (!VulkanResource::createTextureResource(physicalDevice, device, queue, commandPool, gltfResource->specular, specularMap))
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

	if(!ImageDataIO::open(lutMap.imageDataResources, lutFilename))
	{
		return false;
	}

	if (!VulkanResource::createTextureResource(physicalDevice, device, queue, commandPool, gltfResource->lut, lutMap))
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
	if (!initImages(resourceManager, glTF, path))
	{
		return false;
	}

	// Samplers

	if (!initSamplers(resourceManager, glTF))
	{
		return false;
	}

	// Textures

	if (!initTextures(resourceManager, glTF))
	{
		return false;
	}

	// Materials

	if (!initMaterials(resourceManager, glTF))
	{
		return false;
	}

	// Buffers

	if (!initBuffers(resourceManager, glTF))
	{
		return false;
	}

	// BufferViews

	if (!initBufferViews(resourceManager, glTF, useRaytrace))
	{
		return false;
	}

	// Accessors

	if (!initAccessors(resourceManager, glTF))
	{
		return false;
	}

	// Meshes

	if (!initMeshes(resourceManager, glTF, useRaytrace))
	{
		return false;
	}

	// Nodes

	if (!initNodes(resourceManager, glTF, useRaytrace))
	{
		return false;
	}

	// Scenes

	if (!initScenes(resourceManager, glTF, useRaytrace))
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
