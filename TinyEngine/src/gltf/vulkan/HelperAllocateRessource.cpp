#include "HelperAllocateRessource.h"

#include "../../shader/HelperShader.h"
#include "../HelperAccess.h"

#include "HelperAccessResource.h"
#include "ResourceManager.h"

HelperAllocateResource::HelperAllocateResource(uint32_t width, uint32_t height, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, VkRenderPass renderPass, VkSampleCountFlagBits samples, VkImageView imageView) :
	width(width), height(height), physicalDevice(physicalDevice), device(device), queue(queue), commandPool(commandPool), renderPass(renderPass), samples(samples), imageView(imageView)
{
}

bool HelperAllocateResource::initBuffers(ResourceManager& resourceManager, const GLTF& glTF)
{
	return true;
}

bool HelperAllocateResource::initBufferViews(ResourceManager& resourceManager, const GLTF& glTF, bool useRaytrace)
{
	for (size_t i = 0; i < glTF.bufferViews.size(); i++)
	{
		const BufferView& bufferView = glTF.bufferViews[i];

		if (!resourceManager.initBufferView(bufferView, glTF, physicalDevice, device, queue, commandPool, useRaytrace))
		{
			return false;
		}
	}

	return true;
}

bool HelperAllocateResource::initAccessors(ResourceManager& resourceManager, const GLTF& glTF)
{
	return true;
}

bool HelperAllocateResource::initImages(ResourceManager& resourceManager, const GLTF& glTF)
{
	return true;
}

bool HelperAllocateResource::initSamplers(ResourceManager& resourceManager, const GLTF& glTF)
{
	return true;
}

bool HelperAllocateResource::initTextures(ResourceManager& resourceManager, const GLTF& glTF)
{
	for (size_t i = 0; i < glTF.textures.size(); i++)
	{
		const Texture& texture = glTF.textures[i];

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

bool HelperAllocateResource::initMaterials(ResourceManager& resourceManager, const GLTF& glTF)
{
	for (size_t i = 0; i < glTF.materials.size(); i++)
	{
		const Material& material = glTF.materials[i];
		MaterialResource* materialResource = resourceManager.getMaterialResource(&material);

		uint32_t binding = 0;

		std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

		// Metallic Roughness
		if (material.pbrMetallicRoughness.baseColorTexture.index >= 0)
		{
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

		if (material.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0)
		{
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

		// Base Material
		if (material.emissiveTexture.index >= 0)
		{
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

		if (material.occlusionTexture.index >= 0)
		{
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
		}

		if (material.normalTexture.index >= 0)
		{
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
		}

		//
		//

		GltfResource* gltfResource = resourceManager.getGltfResource();

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

bool HelperAllocateResource::initMeshes(ResourceManager& resourceManager, const GLTF& glTF, bool useRaytrace)
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

	for (size_t i = 0; i < glTF.meshes.size(); i++)
	{
		const Mesh& mesh = glTF.meshes[i];

		for (size_t k = 0; k < mesh.primitives.size(); k++)
		{
			const Primitive& primitive = mesh.primitives[k];

			PrimitiveResource* primitiveResource = resourceManager.getPrimitiveResource(&primitive);

			std::map<std::string, std::string> macros;

			VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
			if (primitive.material >= 0)
			{
				macros = resourceManager.getMaterialResource(&glTF.materials[primitive.material])->macros;

				if (glTF.materials[primitive.material].doubleSided)
				{
					cullMode = VK_CULL_MODE_NONE;
				}
			}

			primitiveResource->vertexBuffers.resize(primitive.attributesCount);
			primitiveResource->vertexBuffersOffsets.resize(primitive.attributesCount);

			primitiveResource->vertexInputBindingDescriptions.resize(primitive.attributesCount);
			primitiveResource->vertexInputAttributeDescriptions.resize(primitive.attributesCount);

			uint32_t attributeIndex = 0;

			if (primitive.position >= 0)
			{
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

				macros["POSITION_LOC"] = std::to_string(attributeIndex);

				//

				const Accessor& accessor = glTF.accessors[primitive.position];

				uint32_t stride = HelperAccess::getStride(accessor);
				VkFormat format = VK_FORMAT_UNDEFINED;
				if (!HelperVulkan::getFormat(format, accessor.componentTypeSize, accessor.componentTypeSigned, accessor.componentTypeInteger, accessor.typeCount))
				{
					return false;
				}

				primitiveResource->vertexInputBindingDescriptions[attributeIndex].binding = attributeIndex;
				primitiveResource->vertexInputBindingDescriptions[attributeIndex].stride = stride;
				primitiveResource->vertexInputBindingDescriptions[attributeIndex].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].binding = attributeIndex;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].location = attributeIndex;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].format = format;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].offset = 0;

				//

				primitiveResource->vertexBuffers[attributeIndex] = HelperAccessResource::getBuffer(resourceManager, &accessor);
				primitiveResource->vertexBuffersOffsets[attributeIndex] = HelperAccess::getOffset(accessor);

				//

				attributeIndex++;
			}

			if (primitive.normal >= 0)
			{
				primitiveResource->normalAttributeIndex = attributeIndex;

				if (glTF.accessors[primitive.normal].typeCount == 3)
				{
					macros["NORMAL_VEC3"] = "";
				}
				else
				{
					return false;
				}

				macros["NORMAL_LOC"] = std::to_string(attributeIndex);

				//

				const Accessor& accessor = glTF.accessors[primitive.normal];

				uint32_t stride = HelperAccess::getStride(accessor);
				VkFormat format = VK_FORMAT_UNDEFINED;
				if (!HelperVulkan::getFormat(format, accessor.componentTypeSize, accessor.componentTypeSigned, accessor.componentTypeInteger, accessor.typeCount))
				{
					return false;
				}

				primitiveResource->vertexInputBindingDescriptions[attributeIndex].binding = attributeIndex;
				primitiveResource->vertexInputBindingDescriptions[attributeIndex].stride = stride;
				primitiveResource->vertexInputBindingDescriptions[attributeIndex].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].binding = attributeIndex;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].location = attributeIndex;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].format = format;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].offset = 0;

				//

				primitiveResource->vertexBuffers[attributeIndex] = HelperAccessResource::getBuffer(resourceManager, &accessor);
				primitiveResource->vertexBuffersOffsets[attributeIndex] = HelperAccess::getOffset(accessor);

				//

				attributeIndex++;
			}

			if (primitive.tangent >= 0)
			{
				primitiveResource->tangentAttributeIndex = attributeIndex;

				if (glTF.accessors[primitive.tangent].typeCount == 4)
				{
					macros["TANGENT_VEC4"] = "";
				}
				else
				{
					return false;
				}

				macros["TANGENT_LOC"] = std::to_string(attributeIndex);

				//

				const Accessor& accessor = glTF.accessors[primitive.tangent];

				uint32_t stride = HelperAccess::getStride(accessor);
				VkFormat format = VK_FORMAT_UNDEFINED;
				if (!HelperVulkan::getFormat(format, accessor.componentTypeSize, accessor.componentTypeSigned, accessor.componentTypeInteger, accessor.typeCount))
				{
					return false;
				}

				primitiveResource->vertexInputBindingDescriptions[attributeIndex].binding = attributeIndex;
				primitiveResource->vertexInputBindingDescriptions[attributeIndex].stride = stride;
				primitiveResource->vertexInputBindingDescriptions[attributeIndex].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].binding = attributeIndex;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].location = attributeIndex;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].format = format;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].offset = 0;

				//

				primitiveResource->vertexBuffers[attributeIndex] = HelperAccessResource::getBuffer(resourceManager, &accessor);
				primitiveResource->vertexBuffersOffsets[attributeIndex] = HelperAccess::getOffset(accessor);

				//

				attributeIndex++;
			}

			if (primitive.texCoord0 >= 0)
			{
				primitiveResource->texCoord0AttributeIndex = attributeIndex;

				if (glTF.accessors[primitive.texCoord0].typeCount == 2)
				{
					macros["TEXCOORD_0_VEC2"] = "";
				}
				else
				{
					return false;
				}

				macros["TEXCOORD_0_LOC"] = std::to_string(attributeIndex);

				//

				const Accessor& accessor = glTF.accessors[primitive.texCoord0];

				uint32_t stride = HelperAccess::getStride(accessor);
				VkFormat format = VK_FORMAT_UNDEFINED;
				if (!HelperVulkan::getFormat(format, accessor.componentTypeSize, accessor.componentTypeSigned, accessor.componentTypeInteger, accessor.typeCount))
				{
					return false;
				}

				primitiveResource->vertexInputBindingDescriptions[attributeIndex].binding = attributeIndex;
				primitiveResource->vertexInputBindingDescriptions[attributeIndex].stride = stride;
				primitiveResource->vertexInputBindingDescriptions[attributeIndex].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].binding = attributeIndex;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].location = attributeIndex;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].format = format;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].offset = 0;

				//

				primitiveResource->vertexBuffers[attributeIndex] = HelperAccessResource::getBuffer(resourceManager, &accessor);
				primitiveResource->vertexBuffersOffsets[attributeIndex] = HelperAccess::getOffset(accessor);

				//

				attributeIndex++;
			}

			if (primitive.texCoord1 >= 0)
			{
				primitiveResource->texCoord1AttributeIndex = attributeIndex;

				if (glTF.accessors[primitive.texCoord1].typeCount == 2)
				{
					macros["TEXCOORD_1_VEC2"] = "";
				}
				else
				{
					return false;
				}

				macros["TEXCOORD_1_LOC"] = std::to_string(attributeIndex);

				//

				const Accessor& accessor = glTF.accessors[primitive.texCoord1];

				uint32_t stride = HelperAccess::getStride(accessor);
				VkFormat format = VK_FORMAT_UNDEFINED;
				if (!HelperVulkan::getFormat(format, accessor.componentTypeSize, accessor.componentTypeSigned, accessor.componentTypeInteger, accessor.typeCount))
				{
					return false;
				}

				primitiveResource->vertexInputBindingDescriptions[attributeIndex].binding = attributeIndex;
				primitiveResource->vertexInputBindingDescriptions[attributeIndex].stride = stride;
				primitiveResource->vertexInputBindingDescriptions[attributeIndex].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].binding = attributeIndex;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].location = attributeIndex;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].format = format;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].offset = 0;

				//

				primitiveResource->vertexBuffers[attributeIndex] = HelperAccessResource::getBuffer(resourceManager, &accessor);
				primitiveResource->vertexBuffersOffsets[attributeIndex] = HelperAccess::getOffset(accessor);

				//

				attributeIndex++;
			}

			if (primitive.color0 >= 0)
			{
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

				macros["COLOR_0_LOC"] = std::to_string(attributeIndex);

				//

				const Accessor& accessor = glTF.accessors[primitive.color0];

				uint32_t stride = HelperAccess::getStride(accessor);
				VkFormat format = VK_FORMAT_UNDEFINED;
				if (!HelperVulkan::getFormat(format, accessor.componentTypeSize, accessor.componentTypeSigned, accessor.componentTypeInteger, accessor.typeCount))
				{
					return false;
				}

				primitiveResource->vertexInputBindingDescriptions[attributeIndex].binding = attributeIndex;
				primitiveResource->vertexInputBindingDescriptions[attributeIndex].stride = stride;
				primitiveResource->vertexInputBindingDescriptions[attributeIndex].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].binding = attributeIndex;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].location = attributeIndex;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].format = format;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].offset = 0;

				//

				primitiveResource->vertexBuffers[attributeIndex] = HelperAccessResource::getBuffer(resourceManager, &accessor);
				primitiveResource->vertexBuffersOffsets[attributeIndex] = HelperAccess::getOffset(accessor);

				//

				attributeIndex++;
			}

			if (primitive.joints0 >= 0)
			{
				primitiveResource->joints0AttributeIndex = attributeIndex;

				if (glTF.accessors[primitive.joints0].typeCount == 4)
				{
					macros["JOINTS_0_VEC4"] = "";
				}
				else
				{
					return false;
				}

				macros["JOINTS_0_LOC"] = std::to_string(attributeIndex);

				//

				const Accessor& accessor = glTF.accessors[primitive.joints0];

				uint32_t stride = HelperAccess::getStride(accessor);
				VkFormat format = VK_FORMAT_UNDEFINED;
				if (!HelperVulkan::getFormat(format, accessor.componentTypeSize, accessor.componentTypeSigned, accessor.componentTypeInteger, accessor.typeCount))
				{
					return false;
				}

				primitiveResource->vertexInputBindingDescriptions[attributeIndex].binding = attributeIndex;
				primitiveResource->vertexInputBindingDescriptions[attributeIndex].stride = stride;
				primitiveResource->vertexInputBindingDescriptions[attributeIndex].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].binding = attributeIndex;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].location = attributeIndex;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].format = format;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].offset = 0;

				//

				primitiveResource->vertexBuffers[attributeIndex] = HelperAccessResource::getBuffer(resourceManager, &accessor);
				primitiveResource->vertexBuffersOffsets[attributeIndex] = HelperAccess::getOffset(accessor);

				//

				attributeIndex++;
			}

			if (primitive.joints1 >= 0)
			{
				primitiveResource->joints1AttributeIndex = attributeIndex;

				if (glTF.accessors[primitive.joints1].typeCount == 4)
				{
					macros["JOINTS_1_VEC4"] = "";
				}
				else
				{
					return false;
				}

				macros["JOINTS_1_LOC"] = std::to_string(attributeIndex);

				//

				const Accessor& accessor = glTF.accessors[primitive.joints1];

				uint32_t stride = HelperAccess::getStride(accessor);
				VkFormat format = VK_FORMAT_UNDEFINED;
				if (!HelperVulkan::getFormat(format, accessor.componentTypeSize, accessor.componentTypeSigned, accessor.componentTypeInteger, accessor.typeCount))
				{
					return false;
				}

				primitiveResource->vertexInputBindingDescriptions[attributeIndex].binding = attributeIndex;
				primitiveResource->vertexInputBindingDescriptions[attributeIndex].stride = stride;
				primitiveResource->vertexInputBindingDescriptions[attributeIndex].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].binding = attributeIndex;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].location = attributeIndex;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].format = format;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].offset = 0;

				//

				primitiveResource->vertexBuffers[attributeIndex] = HelperAccessResource::getBuffer(resourceManager, &accessor);
				primitiveResource->vertexBuffersOffsets[attributeIndex] = HelperAccess::getOffset(accessor);

				//

				attributeIndex++;
			}

			if (primitive.weights0 >= 0)
			{
				primitiveResource->weights0AttributeIndex = attributeIndex;

				if (glTF.accessors[primitive.weights0].typeCount == 4)
				{
					macros["WEIGHTS_0_VEC4"] = "";
				}
				else
				{
					return false;
				}

				macros["WEIGHTS_0_LOC"] = std::to_string(attributeIndex);

				//

				const Accessor& accessor = glTF.accessors[primitive.weights0];

				uint32_t stride = HelperAccess::getStride(accessor);
				VkFormat format = VK_FORMAT_UNDEFINED;
				if (!HelperVulkan::getFormat(format, accessor.componentTypeSize, accessor.componentTypeSigned, accessor.componentTypeInteger, accessor.typeCount))
				{
					return false;
				}

				primitiveResource->vertexInputBindingDescriptions[attributeIndex].binding = attributeIndex;
				primitiveResource->vertexInputBindingDescriptions[attributeIndex].stride = stride;
				primitiveResource->vertexInputBindingDescriptions[attributeIndex].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].binding = attributeIndex;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].location = attributeIndex;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].format = format;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].offset = 0;

				//

				primitiveResource->vertexBuffers[attributeIndex] = HelperAccessResource::getBuffer(resourceManager, &accessor);
				primitiveResource->vertexBuffersOffsets[attributeIndex] = HelperAccess::getOffset(accessor);

				//

				attributeIndex++;
			}

			if (primitive.weights1 >= 0)
			{
				primitiveResource->weights1AttributeIndex = attributeIndex;

				if (glTF.accessors[primitive.weights1].typeCount == 4)
				{
					macros["WEIGHTS_1_VEC4"] = "";
				}
				else
				{
					return false;
				}

				macros["WEIGHTS_1_LOC"] = std::to_string(attributeIndex);

				//

				const Accessor& accessor = glTF.accessors[primitive.weights1];

				uint32_t stride = HelperAccess::getStride(accessor);
				VkFormat format = VK_FORMAT_UNDEFINED;
				if (!HelperVulkan::getFormat(format, accessor.componentTypeSize, accessor.componentTypeSigned, accessor.componentTypeInteger, accessor.typeCount))
				{
					return false;
				}

				primitiveResource->vertexInputBindingDescriptions[attributeIndex].binding = attributeIndex;
				primitiveResource->vertexInputBindingDescriptions[attributeIndex].stride = stride;
				primitiveResource->vertexInputBindingDescriptions[attributeIndex].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].binding = attributeIndex;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].location = attributeIndex;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].format = format;
				primitiveResource->vertexInputAttributeDescriptions[attributeIndex].offset = 0;

				//

				primitiveResource->vertexBuffers[attributeIndex] = HelperAccessResource::getBuffer(resourceManager, &accessor);
				primitiveResource->vertexBuffersOffsets[attributeIndex] = HelperAccess::getOffset(accessor);

				//

				attributeIndex++;
			}

			//

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

bool HelperAllocateResource::initNodes(ResourceManager& resourceManager, const GLTF& glTF, bool useRaytrace)
{
	return true;
}

bool HelperAllocateResource::initScenes(ResourceManager& resourceManager, const GLTF& glTF, bool useRaytrace)
{
	for (size_t i = 0; i < glTF.scenes.size(); i++)
	{
		if (!resourceManager.initScene(glTF.scenes[i], glTF, physicalDevice, device, queue, commandPool, imageView, useRaytrace))
		{
			return false;
		}
	}

	return true;
}

bool HelperAllocateResource::allocate(ResourceManager& resourceManager, const GLTF& glTF, const std::string& environment, bool useRaytrace)
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

	//
	//

	// Images

	if (!initImages(resourceManager, glTF))
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

	return true;
}
