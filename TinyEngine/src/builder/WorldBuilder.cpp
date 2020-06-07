#include "../shader/Shader.h"
#include "WorldBuilder.h"

WorldBuilder::WorldBuilder(RenderManager& resourceManager) :
	renderManager(resourceManager)
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

		uint64_t materialHandle = (uint64_t)&material;

		// Metallic Roughness
		if (material.pbrMetallicRoughness.baseColorTexture.index >= 0)
		{
			if (!renderManager.materialSetTexture(materialHandle, textureHandles[material.pbrMetallicRoughness.baseColorTexture.index], "BASECOLOR", material.pbrMetallicRoughness.baseColorTexture.texCoord))
			{
				return false;
			}
		}

		if (material.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0)
		{
			if (!renderManager.materialSetTexture(materialHandle, textureHandles[material.pbrMetallicRoughness.metallicRoughnessTexture.index], "METALLICROUGHNESS", material.pbrMetallicRoughness.metallicRoughnessTexture.texCoord))
			{
				return false;
			}
		}

		// Base Material
		if (material.emissiveTexture.index >= 0)
		{
			if (!renderManager.materialSetTexture(materialHandle, textureHandles[material.emissiveTexture.index], "EMISSIVE", material.emissiveTexture.texCoord))
			{
				return false;
			}
		}

		if (material.occlusionTexture.index >= 0)
		{
			if (!renderManager.materialSetTexture(materialHandle, textureHandles[material.occlusionTexture.index], "OCCLUSION", material.occlusionTexture.texCoord))
			{
				return false;
			}
		}

		if (material.normalTexture.index >= 0)
		{
			if (!renderManager.materialSetTexture(materialHandle, textureHandles[material.normalTexture.index], "NORMAL", material.normalTexture.texCoord))
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

		renderManager.materialSetFactorParameters(materialHandle, materialUniformBuffer);

		if (!renderManager.materialFinalize(materialHandle))
		{
			return false;
		}

		//

		materialHandles.push_back(materialHandle);
	}

	return true;
}

bool WorldBuilder::buildMeshes(const GLTF& glTF)
{
	for (size_t i = 0; i < glTF.meshes.size(); i++)
	{
		const Mesh& mesh = glTF.meshes[i];

		uint64_t groupHandle = (uint64_t)&mesh;

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

				if (!renderManager.geometrySetAttribute(geometryHandle, getBufferHandle(accessor), "POSITION", glTF.accessors[primitive.position].count, format, stride, HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometrySetAttribute(geometryHandle, getBufferHandle(accessor), "NORMAL", glTF.accessors[primitive.normal].count, format, stride, HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometrySetAttribute(geometryHandle, getBufferHandle(accessor), "TANGENT", glTF.accessors[primitive.tangent].count, format, stride, HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometrySetAttribute(geometryHandle, getBufferHandle(accessor), "TEXCOORD_0", glTF.accessors[primitive.texCoord0].count, format, stride, HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometrySetAttribute(geometryHandle, getBufferHandle(accessor), "TEXCOORD_1", glTF.accessors[primitive.texCoord1].count, format, stride, HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometrySetAttribute(geometryHandle, getBufferHandle(accessor), "COLOR_0", glTF.accessors[primitive.color0].count, format, stride, HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometrySetAttribute(geometryHandle, getBufferHandle(accessor), "JOINTS_0", glTF.accessors[primitive.joints0].count, format, stride, HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometrySetAttribute(geometryHandle, getBufferHandle(accessor), "JOINTS_1", glTF.accessors[primitive.joints1].count, format, stride, HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometrySetAttribute(geometryHandle, getBufferHandle(accessor), "WEIGHTS_0", glTF.accessors[primitive.weights0].count, format, stride, HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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

				if (!renderManager.geometrySetAttribute(geometryHandle, getBufferHandle(accessor), "WEIGHTS_1", glTF.accessors[primitive.weights1].count, format, stride, HelperAccess::getOffset(accessor), HelperAccess::getRange(accessor)))
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
					uint64_t targetHandle = (uint64_t)primitive.targetPositionData.data();

					renderManager.sharedDataCreateStorageBuffer(targetHandle, sizeof(glm::vec3) * primitive.targetPositionData.size(), primitive.targetPositionData.data());

					if (!renderManager.sharedDataFinalize(targetHandle))
					{
						return false;
					}

					uint64_t sharedDataHandle = (uint64_t)primitive.targetPositionData.data();

					if (!renderManager.geometryModelSetTarget(geometryModelHandle, sharedDataHandle, "POSITION"))
					{
						return false;
					}
				}

				if (primitive.targetNormalData.size() > 0)
				{
					uint64_t targetHandle = (uint64_t)primitive.targetNormalData.data();

					renderManager.sharedDataCreateStorageBuffer(targetHandle, sizeof(glm::vec3) * primitive.targetNormalData.size(), primitive.targetNormalData.data());

					if (!renderManager.sharedDataFinalize(targetHandle))
					{
						return false;
					}

					uint64_t sharedDataHandle = (uint64_t)primitive.targetNormalData.data();

					if (!renderManager.geometryModelSetTarget(geometryModelHandle, sharedDataHandle, "NORMAL"))
					{
						return false;
					}
				}

				if (primitive.targetTangentData.size() > 0)
				{
					uint64_t targetHandle = (uint64_t)primitive.targetTangentData.data();

					renderManager.sharedDataCreateStorageBuffer(targetHandle, sizeof(glm::vec3) * primitive.targetTangentData.size(), primitive.targetTangentData.data());

					if (!renderManager.sharedDataFinalize(targetHandle))
					{
						return false;
					}

					uint64_t sharedDataHandle = (uint64_t)primitive.targetTangentData.data();

					if (!renderManager.geometryModelSetTarget(geometryModelHandle, sharedDataHandle, "TANGENT"))
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

				if (!renderManager.geometryModelResourceSetIndices(geometryModelHandle, getBufferHandle(glTF.accessors[primitive.indices]), glTF.accessors[primitive.indices].count, indexType, HelperAccess::getOffset(glTF.accessors[primitive.indices]), HelperAccess::getRange(glTF.accessors[primitive.indices])))
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

			if (!renderManager.geometryModelFinalize(geometryModelHandle))
			{
				return false;
			}

			//

			renderManager.groupAddGeometryModel(groupHandle, geometryModelHandle);
		}

		renderManager.groupFinalize(groupHandle);

		//

		meshHandles.push_back(groupHandle);
	}

	return true;
}

bool WorldBuilder::buildNodes(const GLTF& glTF)
{
	for (size_t i = 0; i < glTF.nodes.size(); i++)
	{
		const Node& node = glTF.nodes[i];

		uint64_t nodeHandle = (uint64_t)&node;

		if (node.mesh >= 0)
		{
			renderManager.instanceSetWorldMatrix(nodeHandle, node.worldMatrix);
			renderManager.instanceSetGroup(nodeHandle, meshHandles[node.mesh]);
		}

		renderManager.instanceFinalize(nodeHandle);

		//

		nodeHandles.push_back(nodeHandle);
	}

	return true;
}

bool WorldBuilder::buildScene(const GLTF& glTF)
{
	if (glTF.defaultScene < glTF.scenes.size())
	{
		for (uint64_t nodeHandle : nodeHandles)
		{
			renderManager.worldAddInstance(nodeHandle);
		}

		if (!renderManager.worldFinalize())
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

	if (bufferView.target == 34963) // ELEMENT_ARRAY_BUFFER
	{
		renderManager.sharedDataCreateIndexBuffer(sharedDataHandle, bufferView.byteLength, HelperAccess::accessData(bufferView));
	}
	else
	{
		renderManager.sharedDataCreateVertexBuffer(sharedDataHandle, bufferView.byteLength, HelperAccess::accessData(bufferView));
	}

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

