#include "HelperAccessResource.h"
#include "../render/HelperRasterize.h"

#include "../render/HelperAccessResource.h"

void HelperRasterize::draw(AllocationManager& allocationManager, const Primitive& primitive, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex, DrawMode drawMode, const glm::mat4& worldMatrix)
{
	PrimitiveResource* primitiveResource = allocationManager.getPrimitiveResource(&primitive);

	//

	MaterialResource* materialResource = allocationManager.getResourceManager().getMaterialResource(primitiveResource->materialHandle);
	if (materialResource->alphaMode == 2 && drawMode == OPAQUE)
	{
		return;
	}
	else if (materialResource->alphaMode != 2 && drawMode == TRANSPARENT)
	{
		return;
	}

	//

	WorldResource* worldResource = allocationManager.getWorldResource(&glTF);

	//

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, primitiveResource->graphicsPipeline);

	if (materialResource->descriptorSet != VK_NULL_HANDLE)
	{
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, primitiveResource->pipelineLayout, 0, 1, &materialResource->descriptorSet, 0, nullptr);
	}

	vkCmdPushConstants(commandBuffer, primitiveResource->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(worldResource->viewProjection), &worldResource->viewProjection);
	vkCmdPushConstants(commandBuffer, primitiveResource->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(worldResource->viewProjection), sizeof(worldMatrix), &worldMatrix);

	if (primitiveResource->indexBuffer != VK_NULL_HANDLE)
	{
		vkCmdBindIndexBuffer(commandBuffer, primitiveResource->indexBuffer, primitiveResource->indexOffset, primitiveResource->indexType);
	}

	vkCmdBindVertexBuffers(commandBuffer, 0, primitiveResource->vertexBuffers.size(), primitiveResource->vertexBuffers.data(), primitiveResource->vertexBuffersOffsets.data());

	if (primitiveResource->indexBuffer != VK_NULL_HANDLE)
	{
		vkCmdDrawIndexed(commandBuffer, primitiveResource->count, 1, 0, 0, 0);
	}
	else
	{
		vkCmdDraw(commandBuffer, primitiveResource->count, 1, 0, 0);
	}
}

void HelperRasterize::draw(AllocationManager& allocationManager, const Mesh& mesh, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex, DrawMode drawMode, const glm::mat4& worldMatrix)
{
	for (size_t i = 0; i < mesh.primitives.size(); i++)
	{
		HelperRasterize::draw(allocationManager, mesh.primitives[i], glTF, commandBuffer, frameIndex, drawMode, worldMatrix);
	}
}

void HelperRasterize::draw(AllocationManager& allocationManager, const Node& node, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex, DrawMode drawMode)
{
	if (node.mesh >= 0)
	{
		HelperRasterize::draw(allocationManager, glTF.meshes[node.mesh], glTF, commandBuffer, frameIndex, drawMode, node.worldMatrix);
	}

	for (size_t i = 0; i < node.children.size(); i++)
	{
		HelperRasterize::draw(allocationManager, glTF.nodes[node.children[i]], glTF, commandBuffer, frameIndex, drawMode);
	}
}

void HelperRasterize::draw(AllocationManager& allocationManager, const Scene& scene, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex, DrawMode drawMode)
{
	for (size_t i = 0; i < scene.nodes.size(); i++)
	{
		HelperRasterize::draw(allocationManager, glTF.nodes[scene.nodes[i]], glTF, commandBuffer, frameIndex, drawMode);
	}
}

void HelperRasterize::draw(AllocationManager& allocationManager, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex, DrawMode drawMode)
{
	if (glTF.defaultScene < glTF.scenes.size())
	{
		HelperRasterize::draw(allocationManager, glTF.scenes[glTF.defaultScene], glTF, commandBuffer, frameIndex, drawMode);
	}
}
