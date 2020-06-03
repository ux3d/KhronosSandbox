#include "HelperRasterize.h"

void HelperRasterize::draw(ResourceManager& resourceManager, VkCommandBuffer commandBuffer, uint32_t frameIndex, DrawMode drawMode)
{
	WorldResource* worldResource = resourceManager.getWorldResource();

	for (size_t i = 0; i < worldResource->instanceHandles.size(); i++)
	{
		InstanceResource* instanceResource = resourceManager.getInstanceResource(worldResource->instanceHandles[i]);

		GroupResource* groupResource = resourceManager.getGroupResource(instanceResource->groupHandle);

		for (size_t k = 0; k < groupResource->geometryModelHandles.size(); k++)
		{
			GeometryModelResource* geometryModelResource = resourceManager.getGeometryModelResource(groupResource->geometryModelHandles[k]);

			GeometryResource* geometryResource = resourceManager.getGeometryResource(geometryModelResource->geometryHandle);

			MaterialResource* materialResource = resourceManager.getMaterialResource(geometryModelResource->materialHandle);

			//

			if (materialResource->materialUniformBuffer.alphaMode == 2 && drawMode == OPAQUE)
			{
				return;
			}
			else if (materialResource->materialUniformBuffer.alphaMode != 2 && drawMode == TRANSPARENT)
			{
				return;
			}

			//

			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, geometryModelResource->graphicsPipeline);

			if (materialResource->descriptorSet != VK_NULL_HANDLE)
			{
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, geometryModelResource->pipelineLayout, 0, 1, &materialResource->descriptorSet, 0, nullptr);
			}

			vkCmdPushConstants(commandBuffer, geometryModelResource->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(worldResource->viewProjection), &worldResource->viewProjection);
			vkCmdPushConstants(commandBuffer, geometryModelResource->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(worldResource->viewProjection), sizeof(instanceResource->worldMatrix), &instanceResource->worldMatrix);

			if (geometryModelResource->indexBuffer != VK_NULL_HANDLE)
			{
				vkCmdBindIndexBuffer(commandBuffer, geometryModelResource->indexBuffer, geometryModelResource->indexOffset, geometryModelResource->indexType);
			}

			vkCmdBindVertexBuffers(commandBuffer, 0, static_cast<uint32_t>(geometryResource->vertexBuffers.size()), geometryResource->vertexBuffers.data(), geometryResource->vertexBuffersOffsets.data());

			if (geometryModelResource->indexBuffer != VK_NULL_HANDLE)
			{
				vkCmdDrawIndexed(commandBuffer, geometryModelResource->indicesCount, 1, 0, 0, 0);
			}
			else
			{
				vkCmdDraw(commandBuffer, geometryModelResource->verticesCount, 1, 0, 0);
			}
		}
	}
}