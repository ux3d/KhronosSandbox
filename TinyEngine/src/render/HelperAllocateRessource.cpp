#include "HelperAllocateRessource.h"

#include "../shader/Shader.h"

HelperAllocateResource::HelperAllocateResource(uint32_t width, uint32_t height, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, VkRenderPass renderPass, VkSampleCountFlagBits samples, VkImageView imageView) :
	width(width), height(height), physicalDevice(physicalDevice), device(device), queue(queue), commandPool(commandPool), renderPass(renderPass), samples(samples), imageView(imageView)
{
}

bool HelperAllocateResource::initBufferViews(AllocationManager& allocationManager, const GLTF& glTF, bool useRaytrace)
{
	for (size_t i = 0; i < glTF.bufferViews.size(); i++)
	{
		const BufferView& bufferView = glTF.bufferViews[i];

		if (!allocationManager.createSharedDataResource(bufferView, physicalDevice, device, queue, commandPool, useRaytrace))
		{
			return false;
		}
	}

	return true;
}

bool HelperAllocateResource::initAccessors(AllocationManager& allocationManager, const GLTF& glTF, bool useRaytrace)
{
	for (size_t i = 0; i < glTF.accessors.size(); i++)
	{
		const Accessor& accessor = glTF.accessors[i];

		if (accessor.aliasedBuffer.byteLength > 0)
		{
			if (!allocationManager.createSharedDataResource(accessor.aliasedBufferView, physicalDevice, device, queue, commandPool, useRaytrace))
			{
				return false;
			}
		}
		else if (accessor.sparse.count >= 1)
		{
			if (!allocationManager.createSharedDataResource(accessor.sparse.bufferView, physicalDevice, device, queue, commandPool, useRaytrace))
			{
				return false;
			}
		}
	}

	return true;
}

bool HelperAllocateResource::initTextures(AllocationManager& allocationManager, const GLTF& glTF, bool useRaytrace)
{
	for (size_t i = 0; i < glTF.textures.size(); i++)
	{
		const Texture& texture = glTF.textures[i];

		uint64_t textureHandle = (uint64_t)&texture;

		textureHandles.push_back(textureHandle);

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

		allocationManager.getResourceManager().textureResourceSetCreateInformation(textureHandle, textureResourceCreateInfo);

		if (!allocationManager.getResourceManager().textureResourceFinalize(textureHandle, physicalDevice, device, queue, commandPool))
		{
			return false;
		}
	}

	return true;
}

bool HelperAllocateResource::initMaterials(AllocationManager& allocationManager, const GLTF& glTF, bool useRaytrace)
{
	for (size_t i = 0; i < glTF.materials.size(); i++)
	{
		const Material& material = glTF.materials[i];

		materialHandles.push_back((uint64_t)&material);

		// Metallic Roughness
		if (material.pbrMetallicRoughness.baseColorTexture.index >= 0)
		{
			if (!allocationManager.getResourceManager().materialResourceSetTextureResource(materialHandles[i], textureHandles[material.pbrMetallicRoughness.baseColorTexture.index], material.pbrMetallicRoughness.baseColorTexture.texCoord, "BASECOLOR"))
			{
				return false;
			}
		}

		if (material.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0)
		{
			if (!allocationManager.getResourceManager().materialResourceSetTextureResource(materialHandles[i], textureHandles[material.pbrMetallicRoughness.metallicRoughnessTexture.index], material.pbrMetallicRoughness.metallicRoughnessTexture.texCoord, "METALLICROUGHNESS"))
			{
				return false;
			}
		}

		// Base Material
		if (material.emissiveTexture.index >= 0)
		{
			if (!allocationManager.getResourceManager().materialResourceSetTextureResource(materialHandles[i], textureHandles[material.emissiveTexture.index], material.emissiveTexture.texCoord, "EMISSIVE"))
			{
				return false;
			}
		}

		if (material.occlusionTexture.index >= 0)
		{
			if (!allocationManager.getResourceManager().materialResourceSetTextureResource(materialHandles[i], textureHandles[material.occlusionTexture.index], material.occlusionTexture.texCoord, "OCCLUSION"))
			{
				return false;
			}
		}

		if (material.normalTexture.index >= 0)
		{
			if (!allocationManager.getResourceManager().materialResourceSetTextureResource(materialHandles[i], textureHandles[material.normalTexture.index], material.normalTexture.texCoord, "NORMAL"))
			{
				return false;
			}
		}

		//

		MaterialUniformBuffer materialUniformBuffer = {};

		materialUniformBuffer.baseColorFactor = material.pbrMetallicRoughness.baseColorFactor;

		materialUniformBuffer.metallicFactor = material.pbrMetallicRoughness.metallicFactor;
		materialUniformBuffer.roughnessFactor = material.pbrMetallicRoughness.roughnessFactor;

		materialUniformBuffer.normalScale = material.normalTexture.scale;

		materialUniformBuffer.occlusionStrength = material.occlusionTexture.strength;

		materialUniformBuffer.emissiveFactor = material.emissiveFactor;

		materialUniformBuffer.alphaMode = material.alphaMode;
		materialUniformBuffer.alphaCutoff = material.alphaCutoff;

		materialUniformBuffer.doubleSided = material.doubleSided;

		allocationManager.getResourceManager().materialResourceSetMaterialParameters(materialHandles[i], materialUniformBuffer, physicalDevice, device);

		//
		//

		WorldResource* worldResource = allocationManager.getResourceManager().getWorldResource(glTFHandle);

		MaterialResource* materialResource = allocationManager.getResourceManager().getMaterialResource(materialHandles[i]);

		//
		//

		uint32_t binding = materialResource->binding;

		//

		VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
		descriptorSetLayoutBinding.binding = binding;
		descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding.descriptorCount = 1;
		descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		materialResource->descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

		VkDescriptorImageInfo descriptorImageInfo = {};
		descriptorImageInfo.sampler = worldResource->diffuse.samplerResource.sampler;
		descriptorImageInfo.imageView = worldResource->diffuse.imageViewResource.imageView;
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
		materialResource->descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

		descriptorImageInfo = {};
		descriptorImageInfo.sampler = worldResource->specular.samplerResource.sampler;
		descriptorImageInfo.imageView = worldResource->specular.imageViewResource.imageView;
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
		materialResource->descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

		descriptorImageInfo = {};
		descriptorImageInfo.sampler = worldResource->lut.samplerResource.sampler;
		descriptorImageInfo.imageView = worldResource->lut.imageViewResource.imageView;
		descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		materialResource->descriptorImageInfos.push_back(descriptorImageInfo);

		materialResource->macros["LUT_BINDING"] = std::to_string(binding);

		binding++;

		//

		if (!allocationManager.getResourceManager().materialResourceFinalize(materialHandles[i], device))
		{
			return false;
		}
	}

	return true;
}

bool HelperAllocateResource::initMeshes(AllocationManager& allocationManager, const GLTF& glTF, bool useRaytrace)
{
	for (size_t i = 0; i < glTF.meshes.size(); i++)
	{
		const Mesh& mesh = glTF.meshes[i];

		uint64_t groupHandle = (uint64_t)&mesh;

		meshHandles.push_back(groupHandle);

		for (size_t k = 0; k < mesh.primitives.size(); k++)
		{
			const Primitive& primitive = mesh.primitives[k];

			uint64_t geometryHandle = (uint64_t)&primitive;
			uint64_t geometryModelHandle = (uint64_t)&primitive;

			allocationManager.getResourceManager().geometryModelResourceSetGeometryResource(geometryModelHandle, geometryHandle);

			allocationManager.getResourceManager().groupResourceAddGeometryModelResource(groupHandle, geometryModelHandle);

			std::map<std::string, std::string> macros;

			VkCullModeFlags cullMode = VK_CULL_MODE_NONE;
			if (primitive.material >= 0)
			{
				macros = allocationManager.getResourceManager().getMaterialResource(materialHandles[primitive.material])->macros;
			}

			allocationManager.getResourceManager().geometryResourceSetAttributesCount(geometryHandle, primitive.attributesCount);

			if (primitive.position >= 0)
			{
				const Accessor& accessor = glTF.accessors[primitive.position];

				uint32_t stride = HelperAccess::getStride(accessor);

				VkFormat format = VK_FORMAT_UNDEFINED;
				if (!HelperVulkan::getFormat(format, accessor.componentTypeSize, accessor.componentTypeSigned, accessor.componentTypeInteger, accessor.typeCount, accessor.normalized))
				{
					return false;
				}

				if (!allocationManager.getResourceManager().geometryResourceSetPrimitiveResource(geometryHandle, glTF.accessors[primitive.position].typeCount, "POSITION", macros, format, stride, allocationManager.getResourceManager().getBuffer(allocationManager.getBufferHandle(accessor)), HelperAccess::getOffset(accessor)))
				{
					return false;
				}
			}

			if (primitive.normal >= 0)
			{
				const Accessor& accessor = glTF.accessors[primitive.normal];

				uint32_t stride = HelperAccess::getStride(accessor);

				VkFormat format = VK_FORMAT_UNDEFINED;
				if (!HelperVulkan::getFormat(format, accessor.componentTypeSize, accessor.componentTypeSigned, accessor.componentTypeInteger, accessor.typeCount, accessor.normalized))
				{
					return false;
				}

				if (!allocationManager.getResourceManager().geometryResourceSetPrimitiveResource(geometryHandle, glTF.accessors[primitive.normal].typeCount, "NORMAL", macros, format, stride, allocationManager.getResourceManager().getBuffer(allocationManager.getBufferHandle(accessor)), HelperAccess::getOffset(accessor)))
				{
					return false;
				}
			}

			if (primitive.tangent >= 0)
			{
				const Accessor& accessor = glTF.accessors[primitive.tangent];

				uint32_t stride = HelperAccess::getStride(accessor);

				VkFormat format = VK_FORMAT_UNDEFINED;
				if (!HelperVulkan::getFormat(format, accessor.componentTypeSize, accessor.componentTypeSigned, accessor.componentTypeInteger, accessor.typeCount, accessor.normalized))
				{
					return false;
				}

				if (!allocationManager.getResourceManager().geometryResourceSetPrimitiveResource(geometryHandle, glTF.accessors[primitive.tangent].typeCount, "TANGENT", macros, format, stride, allocationManager.getResourceManager().getBuffer(allocationManager.getBufferHandle(accessor)), HelperAccess::getOffset(accessor)))
				{
					return false;
				}
			}

			if (primitive.texCoord0 >= 0)
			{
				const Accessor& accessor = glTF.accessors[primitive.texCoord0];

				uint32_t stride = HelperAccess::getStride(accessor);

				VkFormat format = VK_FORMAT_UNDEFINED;
				if (!HelperVulkan::getFormat(format, accessor.componentTypeSize, accessor.componentTypeSigned, accessor.componentTypeInteger, accessor.typeCount, accessor.normalized))
				{
					return false;
				}

				if (!allocationManager.getResourceManager().geometryResourceSetPrimitiveResource(geometryHandle, glTF.accessors[primitive.texCoord0].typeCount, "TEXCOORD_0", macros, format, stride, allocationManager.getResourceManager().getBuffer(allocationManager.getBufferHandle(accessor)), HelperAccess::getOffset(accessor)))
				{
					return false;
				}
			}

			if (primitive.texCoord1 >= 0)
			{
				const Accessor& accessor = glTF.accessors[primitive.texCoord1];

				uint32_t stride = HelperAccess::getStride(accessor);

				VkFormat format = VK_FORMAT_UNDEFINED;
				if (!HelperVulkan::getFormat(format, accessor.componentTypeSize, accessor.componentTypeSigned, accessor.componentTypeInteger, accessor.typeCount, accessor.normalized))
				{
					return false;
				}

				if (!allocationManager.getResourceManager().geometryResourceSetPrimitiveResource(geometryHandle, glTF.accessors[primitive.texCoord1].typeCount, "TEXCOORD_1", macros, format, stride, allocationManager.getResourceManager().getBuffer(allocationManager.getBufferHandle(accessor)), HelperAccess::getOffset(accessor)))
				{
					return false;
				}
			}

			if (primitive.color0 >= 0)
			{
				const Accessor& accessor = glTF.accessors[primitive.color0];

				uint32_t stride = HelperAccess::getStride(accessor);

				VkFormat format = VK_FORMAT_UNDEFINED;
				if (!HelperVulkan::getFormat(format, accessor.componentTypeSize, accessor.componentTypeSigned, accessor.componentTypeInteger, accessor.typeCount, accessor.normalized))
				{
					return false;
				}

				if (!allocationManager.getResourceManager().geometryResourceSetPrimitiveResource(geometryHandle, glTF.accessors[primitive.color0].typeCount, "COLOR_0", macros, format, stride, allocationManager.getResourceManager().getBuffer(allocationManager.getBufferHandle(accessor)), HelperAccess::getOffset(accessor)))
				{
					return false;
				}
			}

			if (primitive.joints0 >= 0)
			{
				const Accessor& accessor = glTF.accessors[primitive.joints0];

				uint32_t stride = HelperAccess::getStride(accessor);

				VkFormat format = VK_FORMAT_UNDEFINED;
				if (!HelperVulkan::getFormat(format, accessor.componentTypeSize, accessor.componentTypeSigned, accessor.componentTypeInteger, accessor.typeCount, accessor.normalized))
				{
					return false;
				}

				if (!allocationManager.getResourceManager().geometryResourceSetPrimitiveResource(geometryHandle, glTF.accessors[primitive.joints0].typeCount, "JOINTS_0", macros, format, stride, allocationManager.getResourceManager().getBuffer(allocationManager.getBufferHandle(accessor)), HelperAccess::getOffset(accessor)))
				{
					return false;
				}
			}

			if (primitive.joints1 >= 0)
			{
				const Accessor& accessor = glTF.accessors[primitive.joints1];

				uint32_t stride = HelperAccess::getStride(accessor);

				VkFormat format = VK_FORMAT_UNDEFINED;
				if (!HelperVulkan::getFormat(format, accessor.componentTypeSize, accessor.componentTypeSigned, accessor.componentTypeInteger, accessor.typeCount, accessor.normalized))
				{
					return false;
				}

				if (!allocationManager.getResourceManager().geometryResourceSetPrimitiveResource(geometryHandle, glTF.accessors[primitive.joints1].typeCount, "JOINTS_1", macros, format, stride, allocationManager.getResourceManager().getBuffer(allocationManager.getBufferHandle(accessor)), HelperAccess::getOffset(accessor)))
				{
					return false;
				}
			}

			if (primitive.weights0 >= 0)
			{
				const Accessor& accessor = glTF.accessors[primitive.weights0];

				uint32_t stride = HelperAccess::getStride(accessor);

				VkFormat format = VK_FORMAT_UNDEFINED;
				if (!HelperVulkan::getFormat(format, accessor.componentTypeSize, accessor.componentTypeSigned, accessor.componentTypeInteger, accessor.typeCount, accessor.normalized))
				{
					return false;
				}

				if (!allocationManager.getResourceManager().geometryResourceSetPrimitiveResource(geometryHandle, glTF.accessors[primitive.weights0].typeCount, "WEIGHTS_0", macros, format, stride, allocationManager.getResourceManager().getBuffer(allocationManager.getBufferHandle(accessor)), HelperAccess::getOffset(accessor)))
				{
					return false;
				}
			}

			if (primitive.weights1 >= 0)
			{
				const Accessor& accessor = glTF.accessors[primitive.weights1];

				uint32_t stride = HelperAccess::getStride(accessor);

				VkFormat format = VK_FORMAT_UNDEFINED;
				if (!HelperVulkan::getFormat(format, accessor.componentTypeSize, accessor.componentTypeSigned, accessor.componentTypeInteger, accessor.typeCount, accessor.normalized))
				{
					return false;
				}

				if (!allocationManager.getResourceManager().geometryResourceSetPrimitiveResource(geometryHandle, glTF.accessors[primitive.weights1].typeCount, "WEIGHTS_1", macros, format, stride, allocationManager.getResourceManager().getBuffer(allocationManager.getBufferHandle(accessor)), HelperAccess::getOffset(accessor)))
				{
					return false;
				}
			}

			//

			if (primitive.position < 0)
			{
				return false;
			}

			//

			allocationManager.getResourceManager().geometryModelResourceSetMaterialResource(geometryModelHandle, materialHandles[primitive.material]);

			if (!allocationManager.finalizePrimitive(primitive, glTF, macros, physicalDevice, device, queue, commandPool, width, height, renderPass, samples, &allocationManager.getResourceManager().getMaterialResource(materialHandles[primitive.material])->descriptorSetLayout, cullMode, useRaytrace))
			{
				return false;
			}
		}
	}

	return true;
}

bool HelperAllocateResource::initNodes(AllocationManager& allocationManager, const GLTF& glTF, bool useRaytrace)
{
	for (size_t i = 0; i < glTF.nodes.size(); i++)
	{
		const Node& node = glTF.nodes[i];

		nodeHandles.push_back((uint64_t)&node);

		if (node.mesh >= 0)
		{
			allocationManager.getResourceManager().instanceResourceSetWorldMatrix(nodeHandles[i], node.worldMatrix);
			allocationManager.getResourceManager().instanceResourceSetGroupResource(nodeHandles[i], meshHandles[node.mesh]);
			allocationManager.getResourceManager().instanceResourceFinalize(nodeHandles[i]);
		}
	}

	return true;
}

bool HelperAllocateResource::allocate(AllocationManager& allocationManager, const GLTF& glTF, const std::string& environment, bool useRaytrace)
{
	glTFHandle = (uint64_t)&glTF;

	WorldResource* gltfResource = allocationManager.getResourceManager().getWorldResource(glTFHandle);

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

	// BufferViews

	if (!initBufferViews(allocationManager, glTF, useRaytrace))
	{
		return false;
	}

	// Accessors

	if (!initAccessors(allocationManager, glTF, useRaytrace))
	{
		return false;
	}

	// Textures

	if (!initTextures(allocationManager, glTF, useRaytrace))
	{
		return false;
	}

	// Materials

	if (!initMaterials(allocationManager, glTF, useRaytrace))
	{
		return false;
	}

	// Meshes

	if (!initMeshes(allocationManager, glTF, useRaytrace))
	{
		return false;
	}

	// Nodes

	if (!initNodes(allocationManager, glTF, useRaytrace))
	{
		return false;
	}

	// Scene

	if (glTF.defaultScene < glTF.scenes.size())
	{
		for (size_t i = 0; i < glTF.scenes[glTF.defaultScene].nodes.size(); i++)
		{
			allocationManager.getResourceManager().worldResourceAddInstanceResource(glTFHandle, nodeHandles[glTF.scenes[glTF.defaultScene].nodes[i]]);
		}

		if (!allocationManager.finalizeWorld(glTF, physicalDevice, device, queue, commandPool, imageView, useRaytrace))
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}
