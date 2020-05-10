#include "HelperLoop.h"

#include "../HelperAccess.h"

#include "HelperVulkanAccess.h"

bool HelperLoop::update(Mesh& mesh, GLTF& glTF, const glm::mat4& parentWorldMatrix)
{
	for (size_t i = 0; i < mesh.primitives.size(); i++)
	{
		if (!HelperLoop::update(mesh.primitives[i], glTF, parentWorldMatrix))
		{
			return false;
		}
	}

	return true;
}

void HelperLoop::draw(const Mesh& mesh, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex)
{
	for (size_t i = 0; i < mesh.primitives.size(); i++)
	{
		HelperLoop::draw(mesh.primitives[i], glTF, commandBuffer, frameIndex);
	}
}

bool HelperLoop::update(Node& node, GLTF& glTF, const glm::mat4& parentWorldMatrix)
{
	glm::mat4 matrixTranslation = glm::translate(node.translation);
	glm::mat4 matrixRotation = glm::toMat4(node.rotation);
	glm::mat4 matrixScale = glm::scale(node.scale);

	node.matrix = matrixTranslation * matrixRotation * matrixScale;

	node.worldMatrix = parentWorldMatrix * node.matrix;

	if (node.mesh >= 0)
	{
		if (!HelperLoop::update(glTF.meshes[node.mesh], glTF, node.worldMatrix))
		{
			return false;
		}
	}

	for (size_t i = 0; i < node.children.size(); i++)
	{
		if (!HelperLoop::update(glTF.nodes[node.children[i]], glTF, node.worldMatrix))
		{
			return false;
		}
	}

	return true;
}

void HelperLoop::draw(const Node& node, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex)
{
	if (node.mesh >= 0)
	{
		HelperLoop::draw(glTF.meshes[node.mesh], glTF, commandBuffer, frameIndex);
	}

	for (size_t i = 0; i < node.children.size(); i++)
	{
		HelperLoop::draw(glTF.nodes[node.children[i]], glTF, commandBuffer, frameIndex);
	}
}


bool HelperLoop::update(Primitive& primitive, GLTF& glTF, const glm::mat4& parentWorldMatrix)
{
	primitive.worldMatrix = parentWorldMatrix;

	return true;
}

void HelperLoop::draw(const Primitive& primitive, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, primitive.graphicsPipeline);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, primitive.pipelineLayout, 0, 1, &glTF.materials[primitive.material].descriptorSet, 0, nullptr);

	vkCmdPushConstants(commandBuffer, primitive.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glTF.viewProjection), &glTF.viewProjection);
	vkCmdPushConstants(commandBuffer, primitive.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glTF.viewProjection), sizeof(primitive.worldMatrix), &primitive.worldMatrix);

	if (primitive.indices >= 0)
	{
		VkIndexType indexType = VK_INDEX_TYPE_UINT16;
		if (glTF.accessors[primitive.indices].componentTypeSize == 4)
		{
			indexType = VK_INDEX_TYPE_UINT32;
		}

		vkCmdBindIndexBuffer(commandBuffer, HelperVulkanAccess::getBuffer(glTF.accessors[primitive.indices]), HelperAccess::getOffset(glTF.accessors[primitive.indices]), indexType);
	}

	vkCmdBindVertexBuffers(commandBuffer, 0, primitive.attributesCount, primitive.vertexBuffers.data(), primitive.vertexBuffersOffsets.data());

	if (primitive.indices >= 0)
	{
		vkCmdDrawIndexed(commandBuffer, glTF.accessors[primitive.indices].count, 1, 0, 0, 0);
	}
	else
	{
		vkCmdDraw(commandBuffer, glTF.accessors[primitive.position].count, 1, 0, 0);
	}
}

bool HelperLoop::update(Scene& scene, GLTF& glTF, const glm::mat4& parentWorldMatrix)
{
	for (size_t i = 0; i < scene.nodes.size(); i++)
	{
		if (!HelperLoop::update(glTF.nodes[scene.nodes[i]], glTF, parentWorldMatrix))
		{
			return false;
		}
	}

	return true;
}

void HelperLoop::draw(const Scene& scene, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex)
{
	for (size_t i = 0; i < scene.nodes.size(); i++)
	{
		HelperLoop::draw(glTF.nodes[scene.nodes[i]], glTF, commandBuffer, frameIndex);
	}
}

bool HelperLoop::update(GLTF& glTF, const glm::mat4& parentWorldMatrix)
{
	if (glTF.defaultScene < glTF.scenes.size())
	{
		if (!HelperLoop::update(glTF.scenes[glTF.defaultScene], glTF, parentWorldMatrix))
		{
			return false;
		}
	}

	return true;
}

void HelperLoop::draw(const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex)
{
	if (glTF.defaultScene < glTF.scenes.size())
	{
		HelperLoop::draw(glTF.scenes[glTF.defaultScene], glTF, commandBuffer, frameIndex);
	}
}
