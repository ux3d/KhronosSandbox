#include "HelperRaytrace.h"

void HelperRaytrace::draw(AllocationManager& resourceManager, const Scene& scene, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex, uint32_t width, uint32_t height)
{
	WorldResource* gltfResource = resourceManager.getWorldResource();
	SceneResource* defaultSceneResource = resourceManager.getSceneResource(&scene);

	//

	vkCmdPushConstants(commandBuffer, defaultSceneResource->raytracePipelineLayout, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, 0, sizeof(gltfResource->raytrace), &gltfResource->raytrace);

	VkStridedBufferRegionKHR rayGenStridedBufferRegion = {};
	rayGenStridedBufferRegion.buffer = defaultSceneResource->shaderBindingBufferResource.buffer;
	rayGenStridedBufferRegion.offset = defaultSceneResource->size * 0;
	rayGenStridedBufferRegion.size   = defaultSceneResource->size;

	VkStridedBufferRegionKHR missStridedBufferRegion = {};
	missStridedBufferRegion.buffer = defaultSceneResource->shaderBindingBufferResource.buffer;
	missStridedBufferRegion.offset = defaultSceneResource->size * 1;
	missStridedBufferRegion.size   = defaultSceneResource->size;

	VkStridedBufferRegionKHR closestHitStridedBufferRegion = {};
	closestHitStridedBufferRegion.buffer = defaultSceneResource->shaderBindingBufferResource.buffer;
	closestHitStridedBufferRegion.offset = defaultSceneResource->size * 2;
	closestHitStridedBufferRegion.size   = defaultSceneResource->size;

	VkStridedBufferRegionKHR callableStridedBufferRegion = {};

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, defaultSceneResource->raytracePipeline);

	if (defaultSceneResource->raytraceDescriptorSet != VK_NULL_HANDLE)
	{
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, defaultSceneResource->raytracePipelineLayout, 0, 1, &defaultSceneResource->raytraceDescriptorSet, 0, 0);
	}

	vkCmdTraceRaysKHR(commandBuffer, &rayGenStridedBufferRegion, &missStridedBufferRegion, &closestHitStridedBufferRegion, &callableStridedBufferRegion, width, height, 1);
}

void HelperRaytrace::draw(AllocationManager& resourceManager, const GLTF& glTF, VkCommandBuffer commandBuffer, uint32_t frameIndex, uint32_t width, uint32_t height)
{
	if (glTF.defaultScene < glTF.scenes.size())
	{
		HelperRaytrace::draw(resourceManager, glTF.scenes[glTF.defaultScene], glTF, commandBuffer, frameIndex, width, height);
	}
}
