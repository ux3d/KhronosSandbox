#include "HelperRaytrace.h"

void HelperRaytrace::draw(ResourceManager& resourceManager, VkCommandBuffer commandBuffer, uint32_t frameIndex, uint32_t width, uint32_t height)
{
	WorldResource* worldResource = resourceManager.getWorldResource();

	vkCmdPushConstants(commandBuffer, worldResource->raytracePipelineLayout, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, 0, sizeof(worldResource->raytrace), &worldResource->raytrace);

	VkStridedBufferRegionKHR rayGenStridedBufferRegion = {};
	rayGenStridedBufferRegion.buffer = worldResource->shaderBindingBufferResource.buffer;
	rayGenStridedBufferRegion.offset = worldResource->size * 0;
	rayGenStridedBufferRegion.size   = worldResource->size;

	VkStridedBufferRegionKHR missStridedBufferRegion = {};
	missStridedBufferRegion.buffer = worldResource->shaderBindingBufferResource.buffer;
	missStridedBufferRegion.offset = worldResource->size * 1;
	missStridedBufferRegion.size   = worldResource->size;

	VkStridedBufferRegionKHR closestHitStridedBufferRegion = {};
	closestHitStridedBufferRegion.buffer = worldResource->shaderBindingBufferResource.buffer;
	closestHitStridedBufferRegion.offset = worldResource->size * 2;
	closestHitStridedBufferRegion.size   = worldResource->size;

	VkStridedBufferRegionKHR callableStridedBufferRegion = {};

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, worldResource->raytracePipeline);

	if (worldResource->raytraceDescriptorSet != VK_NULL_HANDLE)
	{
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, worldResource->raytracePipelineLayout, 0, 1, &worldResource->raytraceDescriptorSet, 0, 0);
	}

	vkCmdTraceRaysKHR(commandBuffer, &rayGenStridedBufferRegion, &missStridedBufferRegion, &closestHitStridedBufferRegion, &callableStridedBufferRegion, width, height, 1);
}
