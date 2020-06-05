#include "../shader/Shader.h"
#include "WorldBuilder.h"

WorldBuilder::WorldBuilder(RenderManager& resourceManager, uint32_t width, uint32_t height, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, VkRenderPass renderPass, VkSampleCountFlagBits samples, VkImageView imageView) :
	renderManager(resourceManager), width(width), height(height), physicalDevice(physicalDevice), device(device), queue(queue), commandPool(commandPool), renderPass(renderPass), samples(samples), imageView(imageView)
{
}

bool WorldBuilder::buildBufferViews(const GLTF& glTF, bool useRaytrace)
{
	for (size_t i = 0; i < glTF.bufferViews.size(); i++)
	{
		const BufferView& bufferView = glTF.bufferViews[i];

		if (!createSharedDataResource(bufferView, physicalDevice, device, queue, commandPool, useRaytrace))
		{
			return false;
		}
	}

	return true;
}

bool WorldBuilder::buildAccessors(const GLTF& glTF, bool useRaytrace)
{
	for (size_t i = 0; i < glTF.accessors.size(); i++)
	{
		const Accessor& accessor = glTF.accessors[i];

		if (accessor.aliasedBuffer.byteLength > 0)
		{
			if (!createSharedDataResource(accessor.aliasedBufferView, physicalDevice, device, queue, commandPool, useRaytrace))
			{
				return false;
			}
		}
		else if (accessor.sparse.count >= 1)
		{
			if (!createSharedDataResource(accessor.sparse.bufferView, physicalDevice, device, queue, commandPool, useRaytrace))
			{
				return false;
			}
		}
	}

	return true;
}

bool WorldBuilder::buildTextures(const GLTF& glTF, bool useRaytrace)
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

		renderManager.textureResourceSetCreateInformation(textureHandle, textureResourceCreateInfo);

		if (!renderManager.textureResourceFinalize(textureHandle, physicalDevice, device, queue, commandPool))
		{
			return false;
		}
	}

	return true;
}

bool WorldBuilder::buildMaterials(const GLTF& glTF, bool useRaytrace)
{
	for (size_t i = 0; i < glTF.materials.size(); i++)
	{
		const Material& material = glTF.materials[i];

		materialHandles.push_back((uint64_t)&material);

		// Metallic Roughness
		if (material.pbrMetallicRoughness.baseColorTexture.index >= 0)
		{
			if (!renderManager.materialResourceSetTextureResource(materialHandles[i], textureHandles[material.pbrMetallicRoughness.baseColorTexture.index], material.pbrMetallicRoughness.baseColorTexture.texCoord, "BASECOLOR", material.pbrMetallicRoughness.baseColorTexture.index))
			{
				return false;
			}
		}

		if (material.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0)
		{
			if (!renderManager.materialResourceSetTextureResource(materialHandles[i], textureHandles[material.pbrMetallicRoughness.metallicRoughnessTexture.index], material.pbrMetallicRoughness.metallicRoughnessTexture.texCoord, "METALLICROUGHNESS", material.pbrMetallicRoughness.metallicRoughnessTexture.index))
			{
				return false;
			}
		}

		// Base Material
		if (material.emissiveTexture.index >= 0)
		{
			if (!renderManager.materialResourceSetTextureResource(materialHandles[i], textureHandles[material.emissiveTexture.index], material.emissiveTexture.texCoord, "EMISSIVE", material.emissiveTexture.index))
			{
				return false;
			}
		}

		if (material.occlusionTexture.index >= 0)
		{
			if (!renderManager.materialResourceSetTextureResource(materialHandles[i], textureHandles[material.occlusionTexture.index], material.occlusionTexture.texCoord, "OCCLUSION", material.occlusionTexture.index))
			{
				return false;
			}
		}

		if (material.normalTexture.index >= 0)
		{
			if (!renderManager.materialResourceSetTextureResource(materialHandles[i], textureHandles[material.normalTexture.index], material.normalTexture.texCoord, "NORMAL", material.normalTexture.index))
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

		renderManager.materialResourceSetMaterialParameters(materialHandles[i], static_cast<int32_t>(i), materialUniformBuffer, physicalDevice, device);

		if (!renderManager.materialResourceFinalize(materialHandles[i], device))
		{
			return false;
		}
	}

	return true;
}

bool WorldBuilder::buildMeshes(const GLTF& glTF, bool useRaytrace)
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

			VkCullModeFlags cullMode = VK_CULL_MODE_NONE;

			renderManager.geometryResourceSetAttributesCount(geometryHandle, primitive.attributesCount);

			if (primitive.position >= 0)
			{
				const Accessor& accessor = glTF.accessors[primitive.position];

				uint32_t stride = HelperAccess::getStride(accessor);

				VkFormat format = VK_FORMAT_UNDEFINED;
				if (!HelperVulkan::getFormat(format, accessor.componentTypeSize, accessor.componentTypeSigned, accessor.componentTypeInteger, accessor.typeCount, accessor.normalized))
				{
					return false;
				}

				if (!renderManager.geometryResourceSetAttribute(geometryHandle, glTF.accessors[primitive.position].count, glTF.accessors[primitive.position].typeCount, "POSITION", format, stride, getBufferHandle(accessor), HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometryResourceSetAttribute(geometryHandle, glTF.accessors[primitive.normal].count, glTF.accessors[primitive.normal].typeCount, "NORMAL", format, stride, getBufferHandle(accessor), HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometryResourceSetAttribute(geometryHandle, glTF.accessors[primitive.tangent].count, glTF.accessors[primitive.tangent].typeCount, "TANGENT", format, stride, getBufferHandle(accessor), HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometryResourceSetAttribute(geometryHandle, glTF.accessors[primitive.texCoord0].count, glTF.accessors[primitive.texCoord0].typeCount, "TEXCOORD_0", format, stride, getBufferHandle(accessor), HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometryResourceSetAttribute(geometryHandle, glTF.accessors[primitive.texCoord1].count, glTF.accessors[primitive.texCoord1].typeCount, "TEXCOORD_1", format, stride, getBufferHandle(accessor), HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometryResourceSetAttribute(geometryHandle, glTF.accessors[primitive.color0].count, glTF.accessors[primitive.color0].typeCount, "COLOR_0", format, stride, getBufferHandle(accessor), HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometryResourceSetAttribute(geometryHandle, glTF.accessors[primitive.joints0].count, glTF.accessors[primitive.joints0].typeCount, "JOINTS_0", format, stride, getBufferHandle(accessor), HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometryResourceSetAttribute(geometryHandle, glTF.accessors[primitive.joints1].count, glTF.accessors[primitive.joints1].typeCount, "JOINTS_1", format, stride, getBufferHandle(accessor), HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometryResourceSetAttribute(geometryHandle, glTF.accessors[primitive.weights0].count, glTF.accessors[primitive.weights0].typeCount, "WEIGHTS_0", format, stride, getBufferHandle(accessor), HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometryResourceSetAttribute(geometryHandle, glTF.accessors[primitive.weights1].count, glTF.accessors[primitive.weights1].typeCount, "WEIGHTS_1", format, stride, getBufferHandle(accessor), HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
				{
					return false;
				}
			}

			//

			renderManager.geometryResourceFinalize(geometryHandle);

			//

			if (primitive.position < 0)
			{
				return false;
			}

			//

			renderManager.geometryModelResourceSetGeometryResource(geometryModelHandle, geometryHandle);

			renderManager.geometryModelResourceSetMaterialResource(geometryModelHandle, materialHandles[primitive.material]);

			//

			if (primitive.targetsCount > 0)
			{
				if (primitive.targetPositionData.size() > 0)
				{
					uint64_t sharedDataHandle = (uint64_t)primitive.targetPositionData.data();

					if (!createSharedDataResource(sizeof(glm::vec3) * primitive.targetPositionData.size(), primitive.targetPositionData.data(), physicalDevice, device, queue, commandPool, useRaytrace))
					{
						return false;
					}

					if (!renderManager.geometryModelResourceSetTargetData(geometryModelHandle, "POSITION", sharedDataHandle))
					{
						return false;
					}
				}

				if (primitive.targetNormalData.size() > 0)
				{
					uint64_t sharedDataHandle = (uint64_t)primitive.targetNormalData.data();

					if (!createSharedDataResource(sizeof(glm::vec3) * primitive.targetNormalData.size(), primitive.targetNormalData.data(), physicalDevice, device, queue, commandPool, useRaytrace))
					{
						return false;
					}

					if (!renderManager.geometryModelResourceSetTargetData(geometryModelHandle, "NORMAL", sharedDataHandle))
					{
						return false;
					}
				}

				if (primitive.targetTangentData.size() > 0)
				{
					uint64_t sharedDataHandle = (uint64_t)primitive.targetTangentData.data();

					if (!createSharedDataResource(sizeof(glm::vec3) * primitive.targetTangentData.size(), primitive.targetTangentData.data(), physicalDevice, device, queue, commandPool, useRaytrace))
					{
						return false;
					}

					if (!renderManager.geometryModelResourceSetTargetData(geometryModelHandle, "TANGENT", sharedDataHandle))
					{
						return false;
					}
				}
			}

			//

			VkIndexType indexType = VK_INDEX_TYPE_NONE_KHR;
			if (primitive.indices >= 0)
			{
				indexType = VK_INDEX_TYPE_UINT8_EXT;
				if (glTF.accessors[primitive.indices].componentTypeSize == 2)
				{
					indexType = VK_INDEX_TYPE_UINT16;
				}
				else if (glTF.accessors[primitive.indices].componentTypeSize == 4)
				{
					indexType = VK_INDEX_TYPE_UINT32;
				}

				if (!renderManager.geometryModelResourceSetIndices(geometryModelHandle, glTF.accessors[primitive.indices].count, indexType, getBufferHandle(glTF.accessors[primitive.indices]), HelperAccess::getOffset(glTF.accessors[primitive.indices]), HelperAccess::getRange(glTF.accessors[primitive.indices]), glTF.accessors[primitive.indices].componentTypeSize))
				{
					return false;
				}
			}
			else
			{
				if (!renderManager.geometryModelResourceSetVertexCount(geometryModelHandle, glTF.accessors[primitive.position].count))
				{
					return false;
				}
			}

			if (!renderManager.geometryModelResourceFinalize(geometryModelHandle, width, height, renderPass, cullMode, samples, useRaytrace, physicalDevice, device, queue, commandPool))
			{
				return false;
			}

			//

			renderManager.groupResourceAddGeometryModelResource(groupHandle, geometryModelHandle);
		}

		renderManager.groupResourceFinalize(groupHandle);
	}

	return true;
}

bool WorldBuilder::buildNodes(const GLTF& glTF, bool useRaytrace)
{
	for (size_t i = 0; i < glTF.nodes.size(); i++)
	{
		const Node& node = glTF.nodes[i];

		nodeHandles.push_back((uint64_t)&node);

		if (node.mesh >= 0)
		{
			renderManager.instanceResourceSetWorldMatrix(nodeHandles[i], node.worldMatrix);
			renderManager.instanceResourceSetGroupResource(nodeHandles[i], meshHandles[node.mesh]);
			renderManager.instanceResourceFinalize(nodeHandles[i]);
		}
	}

	return true;
}

bool WorldBuilder::buildScene(const GLTF& glTF, bool useRaytrace)
{
	if (glTF.defaultScene < glTF.scenes.size())
	{
		for (size_t i = 0; i < glTF.scenes[glTF.defaultScene].nodes.size(); i++)
		{
			renderManager.worldResourceAddInstanceResource(nodeHandles[glTF.scenes[glTF.defaultScene].nodes[i]]);
		}

		if (!renderManager.worldResourceFinalize(imageView, useRaytrace, physicalDevice, device, queue, commandPool))
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

bool WorldBuilder::build(const GLTF& glTF, const std::string& environment, bool useRaytrace)
{
	glTFHandle = (uint64_t)&glTF;

	renderManager.lightResourceSetEnvironmentLight(glTFHandle, environment);
	renderManager.lightResourceFinalize(glTFHandle, physicalDevice, device, queue, commandPool);

	renderManager.cameraResourceFinalize(glTFHandle);

	renderManager.worldResourceSetLightResource(glTFHandle);
	renderManager.worldResourceSetCameraResource(glTFHandle);

	// BufferViews

	if (!buildBufferViews(glTF, useRaytrace))
	{
		return false;
	}

	// Accessors

	if (!buildAccessors(glTF, useRaytrace))
	{
		return false;
	}

	// Textures

	if (!buildTextures(glTF, useRaytrace))
	{
		return false;
	}

	// Materials

	if (!buildMaterials(glTF, useRaytrace))
	{
		return false;
	}

	// Meshes

	if (!buildMeshes(glTF, useRaytrace))
	{
		return false;
	}

	// Nodes

	if (!buildNodes(glTF, useRaytrace))
	{
		return false;
	}

	// Scene

	if (!buildScene(glTF, useRaytrace))
	{
		return false;
	}

	return true;
}


uint64_t WorldBuilder::getBufferHandle(const Accessor& accessor)
{
	uint64_t sharedDataHandle = 0;

	if (accessor.aliasedBuffer.byteLength > 0)
	{
		sharedDataHandle = (uint64_t)&accessor.aliasedBufferView;

		return sharedDataHandle;
	}

	if (accessor.sparse.count >= 1)
	{
		sharedDataHandle = (uint64_t)&accessor.sparse.bufferView;

		return sharedDataHandle;
	}

	if (accessor.pBufferView == nullptr)
	{
		return sharedDataHandle;
	}

	sharedDataHandle = (uint64_t)accessor.pBufferView;

	return sharedDataHandle;
}

bool WorldBuilder::createSharedDataResource(const BufferView& bufferView, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, bool useRaytrace)
{
	uint64_t sharedDataHandle = (uint64_t)&bufferView;

	VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	if (bufferView.target == 34963) // ELEMENT_ARRAY_BUFFER
	{
		usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	}
	if (useRaytrace)
	{
		usage |= (VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
	}
	renderManager.sharedDataSetUsage(sharedDataHandle, usage);

	renderManager.sharedDataSetData(sharedDataHandle, bufferView.byteLength, HelperAccess::accessData(bufferView));

	if (!renderManager.sharedDataResourceFinalize(sharedDataHandle, physicalDevice, device, queue, commandPool))
	{
		return false;
	}

	return true;
}

bool WorldBuilder::createSharedDataResource(VkDeviceSize size, const void* data, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool, bool useRaytrace)
{
	uint64_t sharedDataHandle = (uint64_t)data;

	VkBufferUsageFlags usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	if (useRaytrace)
	{
		usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
	}
	renderManager.sharedDataSetUsage(sharedDataHandle, usage);

	renderManager.sharedDataSetData(sharedDataHandle, size, data);

	if (!renderManager.sharedDataResourceFinalize(sharedDataHandle, physicalDevice, device, queue, commandPool))
	{
		return false;
	}

	return true;
}

void WorldBuilder::terminate(VkDevice device)
{
	renderManager.terminate(device);
}

