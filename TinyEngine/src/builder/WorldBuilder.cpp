#include "../shader/Shader.h"
#include "WorldBuilder.h"

WorldBuilder::WorldBuilder(RenderManager& resourceManager, VkRenderPass renderPass, VkSampleCountFlagBits samples, VkImageView imageView) :
	renderManager(resourceManager), renderPass(renderPass), samples(samples), imageView(imageView)
{
}

bool WorldBuilder::buildBufferViews(const GLTF& glTF)
{
	for (size_t i = 0; i < glTF.bufferViews.size(); i++)
	{
		const BufferView& bufferView = glTF.bufferViews[i];

		if (!createSharedDataResource(bufferView))
		{
			return false;
		}
	}

	return true;
}

bool WorldBuilder::buildAccessors(const GLTF& glTF)
{
	for (size_t i = 0; i < glTF.accessors.size(); i++)
	{
		const Accessor& accessor = glTF.accessors[i];

		if (accessor.aliasedBuffer.byteLength > 0)
		{
			if (!createSharedDataResource(accessor.aliasedBufferView))
			{
				return false;
			}
		}
		else if (accessor.sparse.count >= 1)
		{
			if (!createSharedDataResource(accessor.sparse.bufferView))
			{
				return false;
			}
		}
	}

	return true;
}

bool WorldBuilder::buildTextures(const GLTF& glTF)
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

		renderManager.textureSetParameters(textureHandle, textureResourceCreateInfo);

		if (!renderManager.textureFinalize(textureHandle))
		{
			return false;
		}
	}

	return true;
}

bool WorldBuilder::buildMaterials(const GLTF& glTF)
{
	for (size_t i = 0; i < glTF.materials.size(); i++)
	{
		const Material& material = glTF.materials[i];

		materialHandles.push_back((uint64_t)&material);

		// Metallic Roughness
		if (material.pbrMetallicRoughness.baseColorTexture.index >= 0)
		{
			if (!renderManager.materialSetTexture(materialHandles[i], textureHandles[material.pbrMetallicRoughness.baseColorTexture.index], material.pbrMetallicRoughness.baseColorTexture.texCoord, "BASECOLOR", material.pbrMetallicRoughness.baseColorTexture.index))
			{
				return false;
			}
		}

		if (material.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0)
		{
			if (!renderManager.materialSetTexture(materialHandles[i], textureHandles[material.pbrMetallicRoughness.metallicRoughnessTexture.index], material.pbrMetallicRoughness.metallicRoughnessTexture.texCoord, "METALLICROUGHNESS", material.pbrMetallicRoughness.metallicRoughnessTexture.index))
			{
				return false;
			}
		}

		// Base Material
		if (material.emissiveTexture.index >= 0)
		{
			if (!renderManager.materialSetTexture(materialHandles[i], textureHandles[material.emissiveTexture.index], material.emissiveTexture.texCoord, "EMISSIVE", material.emissiveTexture.index))
			{
				return false;
			}
		}

		if (material.occlusionTexture.index >= 0)
		{
			if (!renderManager.materialSetTexture(materialHandles[i], textureHandles[material.occlusionTexture.index], material.occlusionTexture.texCoord, "OCCLUSION", material.occlusionTexture.index))
			{
				return false;
			}
		}

		if (material.normalTexture.index >= 0)
		{
			if (!renderManager.materialSetTexture(materialHandles[i], textureHandles[material.normalTexture.index], material.normalTexture.texCoord, "NORMAL", material.normalTexture.index))
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

		renderManager.materialSetFactorParameters(materialHandles[i], static_cast<int32_t>(i), materialUniformBuffer);

		if (!renderManager.materialFinalize(materialHandles[i]))
		{
			return false;
		}
	}

	return true;
}

bool WorldBuilder::buildMeshes(const GLTF& glTF)
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

			if (primitive.position >= 0)
			{
				const Accessor& accessor = glTF.accessors[primitive.position];

				uint32_t stride = HelperAccess::getStride(accessor);

				VkFormat format = VK_FORMAT_UNDEFINED;
				if (!HelperVulkan::getFormat(format, accessor.componentTypeSize, accessor.componentTypeSigned, accessor.componentTypeInteger, accessor.typeCount, accessor.normalized))
				{
					return false;
				}

				if (!renderManager.geometrySetAttribute(geometryHandle, glTF.accessors[primitive.position].count, glTF.accessors[primitive.position].typeCount, "POSITION", format, stride, getBufferHandle(accessor), HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometrySetAttribute(geometryHandle, glTF.accessors[primitive.normal].count, glTF.accessors[primitive.normal].typeCount, "NORMAL", format, stride, getBufferHandle(accessor), HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometrySetAttribute(geometryHandle, glTF.accessors[primitive.tangent].count, glTF.accessors[primitive.tangent].typeCount, "TANGENT", format, stride, getBufferHandle(accessor), HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometrySetAttribute(geometryHandle, glTF.accessors[primitive.texCoord0].count, glTF.accessors[primitive.texCoord0].typeCount, "TEXCOORD_0", format, stride, getBufferHandle(accessor), HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometrySetAttribute(geometryHandle, glTF.accessors[primitive.texCoord1].count, glTF.accessors[primitive.texCoord1].typeCount, "TEXCOORD_1", format, stride, getBufferHandle(accessor), HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometrySetAttribute(geometryHandle, glTF.accessors[primitive.color0].count, glTF.accessors[primitive.color0].typeCount, "COLOR_0", format, stride, getBufferHandle(accessor), HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometrySetAttribute(geometryHandle, glTF.accessors[primitive.joints0].count, glTF.accessors[primitive.joints0].typeCount, "JOINTS_0", format, stride, getBufferHandle(accessor), HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometrySetAttribute(geometryHandle, glTF.accessors[primitive.joints1].count, glTF.accessors[primitive.joints1].typeCount, "JOINTS_1", format, stride, getBufferHandle(accessor), HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometrySetAttribute(geometryHandle, glTF.accessors[primitive.weights0].count, glTF.accessors[primitive.weights0].typeCount, "WEIGHTS_0", format, stride, getBufferHandle(accessor), HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometrySetAttribute(geometryHandle, glTF.accessors[primitive.weights1].count, glTF.accessors[primitive.weights1].typeCount, "WEIGHTS_1", format, stride, getBufferHandle(accessor), HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
				{
					return false;
				}
			}

			//

			renderManager.geometryFinalize(geometryHandle);

			//

			if (primitive.position < 0)
			{
				return false;
			}

			//

			renderManager.geometryModelSetGeometry(geometryModelHandle, geometryHandle);

			renderManager.geometryModelSetMaterial(geometryModelHandle, materialHandles[primitive.material]);

			//

			if (primitive.targetsCount > 0)
			{
				if (primitive.targetPositionData.size() > 0)
				{
					uint64_t sharedDataHandle = (uint64_t)primitive.targetPositionData.data();

					if (!createSharedDataResource(sizeof(glm::vec3) * primitive.targetPositionData.size(), primitive.targetPositionData.data()))
					{
						return false;
					}

					if (!renderManager.geometryModelSetTarget(geometryModelHandle, "POSITION", sharedDataHandle))
					{
						return false;
					}
				}

				if (primitive.targetNormalData.size() > 0)
				{
					uint64_t sharedDataHandle = (uint64_t)primitive.targetNormalData.data();

					if (!createSharedDataResource(sizeof(glm::vec3) * primitive.targetNormalData.size(), primitive.targetNormalData.data()))
					{
						return false;
					}

					if (!renderManager.geometryModelSetTarget(geometryModelHandle, "NORMAL", sharedDataHandle))
					{
						return false;
					}
				}

				if (primitive.targetTangentData.size() > 0)
				{
					uint64_t sharedDataHandle = (uint64_t)primitive.targetTangentData.data();

					if (!createSharedDataResource(sizeof(glm::vec3) * primitive.targetTangentData.size(), primitive.targetTangentData.data()))
					{
						return false;
					}

					if (!renderManager.geometryModelSetTarget(geometryModelHandle, "TANGENT", sharedDataHandle))
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
				if (!renderManager.geometryModelSetVertexCount(geometryModelHandle, glTF.accessors[primitive.position].count))
				{
					return false;
				}
			}

			if (!renderManager.geometryModelSetCullMode(geometryModelHandle, VK_CULL_MODE_BACK_BIT))
			{
				return false;
			}

			if (!renderManager.geometryModelFinalize(geometryModelHandle, renderPass, samples))
			{
				return false;
			}

			//

			renderManager.groupAddGeometryModel(groupHandle, geometryModelHandle);
		}

		renderManager.groupFinalize(groupHandle);
	}

	return true;
}

bool WorldBuilder::buildNodes(const GLTF& glTF)
{
	for (size_t i = 0; i < glTF.nodes.size(); i++)
	{
		const Node& node = glTF.nodes[i];

		nodeHandles.push_back((uint64_t)&node);

		if (node.mesh >= 0)
		{
			renderManager.instanceSetWorldMatrix(nodeHandles[i], node.worldMatrix);
			renderManager.instanceSetGroup(nodeHandles[i], meshHandles[node.mesh]);
			renderManager.instanceFinalize(nodeHandles[i]);
		}
	}

	return true;
}

bool WorldBuilder::buildScene(const GLTF& glTF)
{
	if (glTF.defaultScene < glTF.scenes.size())
	{
		for (size_t i = 0; i < glTF.scenes[glTF.defaultScene].nodes.size(); i++)
		{
			renderManager.worldAddInstance(nodeHandles[glTF.scenes[glTF.defaultScene].nodes[i]]);
		}

		if (!renderManager.worldFinalize(imageView))
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

bool WorldBuilder::build(const GLTF& glTF, const std::string& environment)
{
	glTFHandle = (uint64_t)&glTF;

	renderManager.lightSetEnvironment(glTFHandle, environment);
	renderManager.lightFinalize(glTFHandle);

	renderManager.cameraFinalize(glTFHandle);

	renderManager.worldSetLight(glTFHandle);
	renderManager.worldSetCamera(glTFHandle);

	// BufferViews

	if (!buildBufferViews(glTF))
	{
		return false;
	}

	// Accessors

	if (!buildAccessors(glTF))
	{
		return false;
	}

	// Textures

	if (!buildTextures(glTF))
	{
		return false;
	}

	// Materials

	if (!buildMaterials(glTF))
	{
		return false;
	}

	// Meshes

	if (!buildMeshes(glTF))
	{
		return false;
	}

	// Nodes

	if (!buildNodes(glTF))
	{
		return false;
	}

	// Scene

	if (!buildScene(glTF))
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

bool WorldBuilder::createSharedDataResource(const BufferView& bufferView)
{
	uint64_t sharedDataHandle = (uint64_t)&bufferView;

	VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	if (bufferView.target == 34963) // ELEMENT_ARRAY_BUFFER
	{
		usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	}
	renderManager.sharedDataSetData(sharedDataHandle, bufferView.byteLength, HelperAccess::accessData(bufferView), usage);

	if (!renderManager.sharedDataFinalize(sharedDataHandle))
	{
		return false;
	}

	return true;
}

bool WorldBuilder::createSharedDataResource(VkDeviceSize size, const void* data)
{
	uint64_t sharedDataHandle = (uint64_t)data;

	renderManager.sharedDataSetData(sharedDataHandle, size, data, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

	if (!renderManager.sharedDataFinalize(sharedDataHandle))
	{
		return false;
	}

	return true;
}

void WorldBuilder::terminate()
{
	renderManager.terminate();
}

