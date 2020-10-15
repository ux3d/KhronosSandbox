#include "../shader/Shader.h"
#include "WorldBuilder.h"

WorldBuilder::WorldBuilder(const GLTF& glTF, const std::string& environment, RenderManager& resourceManager) :
	glTF(glTF), environment(environment), renderManager(resourceManager)
{
}

bool WorldBuilder::buildBufferViews()
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

bool WorldBuilder::buildAccessors()
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

bool WorldBuilder::buildTextures()
{
	for (size_t i = 0; i < glTF.textures.size(); i++)
	{
		const Texture& texture = glTF.textures[i];

		uint64_t textureHandle;
		if (!renderManager.textureCreate(textureHandle))
		{
			return false;
		}

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

		if (!renderManager.textureSetParameters(textureHandle, textureResourceCreateInfo))
		{
			return false;
		}

		if (!renderManager.textureFinalize(textureHandle))
		{
			return false;
		}

		//

		textureHandles.push_back(textureHandle);
	}

	return true;
}

bool WorldBuilder::buildMaterials()
{
	for (size_t i = 0; i < glTF.materials.size(); i++)
	{
		const Material& material = glTF.materials[i];

		uint64_t materialHandle;
		if (!renderManager.materialCreate(materialHandle))
		{
			return false;
		}

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

		MaterialParameters materialParameters = {};

		materialParameters.baseColorFactor = material.pbrMetallicRoughness.baseColorFactor;

		materialParameters.metallicFactor = material.pbrMetallicRoughness.metallicFactor;
		materialParameters.roughnessFactor = material.pbrMetallicRoughness.roughnessFactor;

		materialParameters.normalScale = material.normalTexture.scale;

		materialParameters.occlusionStrength = material.occlusionTexture.strength;

		materialParameters.emissiveFactor = material.emissiveFactor;

		materialParameters.alphaMode = material.alphaMode;
		materialParameters.alphaCutoff = material.alphaCutoff;

		materialParameters.doubleSided = material.doubleSided;

		if (!renderManager.materialSetParameters(materialHandle, materialParameters))
		{
			return false;
		}

		if (!renderManager.materialFinalize(materialHandle))
		{
			return false;
		}

		//

		materialHandles.push_back(materialHandle);
	}

	return true;
}

bool WorldBuilder::buildMeshes()
{
	for (size_t i = 0; i < glTF.meshes.size(); i++)
	{
		const Mesh& mesh = glTF.meshes[i];

		uint64_t groupHandle;
		if (!renderManager.groupCreate(groupHandle))
		{
			return false;
		}

		uint32_t weightsCount = 0;

		for (size_t k = 0; k < mesh.primitives.size(); k++)
		{
			const Primitive& primitive = mesh.primitives[k];

			uint64_t geometryHandle;
			if (!renderManager.geometryCreate(geometryHandle))
			{
				return false;
			}

			uint64_t geometryModelHandle;
			if (!renderManager.geometryModelCreate(geometryModelHandle))
			{
				return false;
			}

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

			if (!renderManager.geometryFinalize(geometryHandle))
			{
				return false;
			}

			//

			if (primitive.position < 0)
			{
				return false;
			}

			//

			if (!renderManager.geometryModelSetGeometry(geometryModelHandle, geometryHandle))
			{
				return false;
			}

			if (!renderManager.geometryModelSetMaterial(geometryModelHandle, materialHandles[primitive.material]))
			{
				return false;
			}

			//

			if (primitive.targets.size() > 0)
			{
				if (primitive.targetPositionData.size() > 0)
				{
					uint64_t targetHandle;
					if (!renderManager.sharedDataCreate(targetHandle))
					{
						return false;
					}

					if (!renderManager.sharedDataCreateStorageBuffer(targetHandle, sizeof(glm::vec3) * primitive.targetPositionData.size(), primitive.targetPositionData.data()))
					{
						return false;
					}

					if (!renderManager.sharedDataFinalize(targetHandle))
					{
						return false;
					}

					if (!renderManager.geometryModelSetTarget(geometryModelHandle, targetHandle, "POSITION"))
					{
						return false;
					}
				}

				if (primitive.targetNormalData.size() > 0)
				{
					uint64_t targetHandle;
					if (!renderManager.sharedDataCreate(targetHandle))
					{
						return false;
					}

					if (!renderManager.sharedDataCreateStorageBuffer(targetHandle, sizeof(glm::vec3) * primitive.targetNormalData.size(), primitive.targetNormalData.data()))
					{
						return false;
					}

					if (!renderManager.sharedDataFinalize(targetHandle))
					{
						return false;
					}

					if (!renderManager.geometryModelSetTarget(geometryModelHandle, targetHandle, "NORMAL"))
					{
						return false;
					}
				}

				if (primitive.targetTangentData.size() > 0)
				{
					uint64_t targetHandle;
					if (!renderManager.sharedDataCreate(targetHandle))
					{
						return false;
					}

					if (!renderManager.sharedDataCreateStorageBuffer(targetHandle, sizeof(glm::vec3) * primitive.targetTangentData.size(), primitive.targetTangentData.data()))
					{
						return false;
					}

					if (!renderManager.sharedDataFinalize(targetHandle))
					{
						return false;
					}

					if (!renderManager.geometryModelSetTarget(geometryModelHandle, targetHandle, "TANGENT"))
					{
						return false;
					}
				}

				//

				if (!renderManager.geometryModelSetTargetsCount(geometryModelHandle, static_cast<uint32_t>(primitive.targets.size())))
				{
					return false;
				}

				//

				weightsCount += primitive.targets.size();
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

				if (!renderManager.geometryModelSetIndices(geometryModelHandle, getBufferHandle(glTF.accessors[primitive.indices]), glTF.accessors[primitive.indices].count, indexType, HelperAccess::getOffset(glTF.accessors[primitive.indices]), HelperAccess::getRange(glTF.accessors[primitive.indices])))
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

			if (!renderManager.groupAddGeometryModel(groupHandle, geometryModelHandle))
			{
				return false;
			}
		}

		//

		if (weightsCount > 0)
		{
			std::vector<float> weights(weightsCount, 0.0f);

			if (mesh.weights.size() > 0)
			{
				if (weights.size() != mesh.weights.size())
				{
					return false;
				}

				memcpy(weights.data(), mesh.weights.data(), mesh.weights.size()* sizeof(float));
			}

			// TODO: Set weights.
		}

		//

		if (!renderManager.groupFinalize(groupHandle))
		{
			return false;
		}


		//

		groupHandles.push_back(groupHandle);
	}

	return true;
}

bool WorldBuilder::buildNodes()
{
	for (size_t i = 0; i < glTF.nodes.size(); i++)
	{
		const Node& node = glTF.nodes[i];

		uint64_t instanceHandle;
		if (!renderManager.instanceCreate(instanceHandle))
		{
			return false;
		}

		nodeToHandles[&node] = instanceHandle;

		if (node.mesh >= 0)
		{
			if (!renderManager.instanceSetWorldMatrix(instanceHandle, node.worldMatrix))
			{
				return false;
			}

			if (!renderManager.instanceSetGroup(instanceHandle, groupHandles[node.mesh]))
			{
				return false;
			}

		}

		if (!renderManager.instanceFinalize(instanceHandle))
		{
			return false;
		}

		//

		instanceHandles.push_back(instanceHandle);
	}

	return true;
}

bool WorldBuilder::buildScene()
{
	if (glTF.defaultScene < glTF.scenes.size())
	{
		for (uint64_t instanceHandle : instanceHandles)
		{
			if (!renderManager.worldAddInstance(instanceHandle))
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

bool WorldBuilder::build()
{
	if (!renderManager.worldCreate())
	{
		return false;
	}

	//

	uint64_t lightHandle;
	if (!renderManager.lightCreate(lightHandle))
	{
		return false;
	}

	if (!renderManager.lightSetEnvironment(lightHandle, environment))
	{
		return false;
	}

	if (!renderManager.lightFinalize(lightHandle))
	{
		return false;
	}

	//

	uint64_t cameraHandle;
	if (!renderManager.cameraCreate(cameraHandle))
	{
		return false;
	}

	if (!renderManager.cameraFinalize(cameraHandle))
	{
		return false;
	}

	//

	if (!renderManager.worldSetLight(lightHandle))
	{
		return false;
	}

	if (!renderManager.worldSetCamera(cameraHandle))
	{
		return false;
	}

	// BufferViews

	if (!buildBufferViews())
	{
		return false;
	}

	// Accessors

	if (!buildAccessors())
	{
		return false;
	}

	// Textures

	if (!buildTextures())
	{
		return false;
	}

	// Materials

	if (!buildMaterials())
	{
		return false;
	}

	// Meshes

	if (!buildMeshes())
	{
		return false;
	}

	// Nodes

	if (!buildNodes())
	{
		return false;
	}

	// Scene

	if (!buildScene())
	{
		return false;
	}

	if (!renderManager.worldFinalize())
	{
		return false;
	}

	return true;
}


uint64_t WorldBuilder::getBufferHandle(const Accessor& accessor)
{
	if (accessor.aliasedBuffer.byteLength > 0)
	{
		return bufferViewToHandle[&accessor.aliasedBufferView];
	}

	if (accessor.sparse.count >= 1)
	{
		return bufferViewToHandle[&accessor.sparse.bufferView];
	}

	if (accessor.pBufferView == nullptr)
	{
		return 0;
	}

	return bufferViewToHandle[accessor.pBufferView];
}

bool WorldBuilder::createSharedDataResource(const BufferView& bufferView)
{
	uint64_t sharedDataHandle;
	if (!renderManager.sharedDataCreate(sharedDataHandle))
	{
		return false;
	}

	if (bufferView.target == 34963) // ELEMENT_ARRAY_BUFFER
	{
		if (!renderManager.sharedDataCreateIndexBuffer(sharedDataHandle, bufferView.byteLength, HelperAccess::accessData(bufferView)))
		{
			return false;
		}
	}
	else
	{
		if (!renderManager.sharedDataCreateVertexBuffer(sharedDataHandle, bufferView.byteLength, HelperAccess::accessData(bufferView)))
		{
			return false;
		}
	}

	if (!renderManager.sharedDataFinalize(sharedDataHandle))
	{
		return false;
	}

	bufferViewToHandle[&bufferView] = sharedDataHandle;

	return true;
}

std::map<const Node*, uint64_t> WorldBuilder::cloneNodeToHandles() const
{
	return nodeToHandles;
}
