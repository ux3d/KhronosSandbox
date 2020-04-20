#include "Application.h"

#include "GLTF/HelperAccess.h"
#include "GLTF/HelperLoader.h"
#include "GLTF/HelperLoop.h"
#include "GLTF/HelperResource.h"

// Private

bool Application::applicationInit()
{
	//
	// Raytracing output buffer
	//

	ImageViewResourceCreateInfo imageViewResourceCreateInfo = {};
	imageViewResourceCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
	imageViewResourceCreateInfo.extent = {width, height, 1};
	imageViewResourceCreateInfo.mipLevels = 1;
	imageViewResourceCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageViewResourceCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
	imageViewResourceCreateInfo.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

	if (!Helper::createImageViewResource(physicalDevice, device, raytraceImageViewResource, imageViewResourceCreateInfo))
	{
		return false;
	}

	HelperLoader helperLoader(width, height, physicalDevice, device, queue, commandPool, renderPass, samples, raytraceImageViewResource.imageView);

	if(!helperLoader.open(glTF, filename, environment, true))
	{
		return false;
	}

	return true;
}

bool Application::applicationUpdate(uint32_t frameIndex, double deltaTime, double totalTime)
{
	ImGui_ImplVulkan_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Settings");
	ImGui::Checkbox("Raytrace", &raytrace);
	if (raytrace)
	{
		int temp = (int)maxDepth;
		ImGui::InputInt("Max depth", &temp);
		if (temp >= 0 && temp <= 1)
		{
			maxDepth = (uint32_t)temp;
		}
		if (maxDepth > 0)
		{
			temp = (int)specularSamples;
			ImGui::SliderInt("Specular samples", &temp, 1, 128);
			specularSamples = (uint32_t)temp;

			temp = (int)diffuseSamples;
			ImGui::SliderInt("Diffuse samples", &temp, 1, 128);
			diffuseSamples = (uint32_t)temp;
		}
	}
	ImGui::Separator();
	ImGui::SliderFloat("Zoom Speed", &zoomSpeed, 0.01f, 0.1f, "ratio = %.2f");
	ImGui::End();

	ImGui::Render();

	//
	// Rendering
	//

	if (raytrace)
	{
		VkImageMemoryBarrier imageMemoryBarrier[2] = {};

		imageMemoryBarrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageMemoryBarrier[0].subresourceRange.levelCount = 1;
		imageMemoryBarrier[0].subresourceRange.layerCount = 1;
		imageMemoryBarrier[0].image = raytraceImageViewResource.image;
		//
		imageMemoryBarrier[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier[1].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageMemoryBarrier[1].subresourceRange.levelCount = 1;
		imageMemoryBarrier[1].subresourceRange.layerCount = 1;
		imageMemoryBarrier[1].image = swapchainImages[frameIndex];

		// Prepare to raytrace into image.

		imageMemoryBarrier[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		imageMemoryBarrier[0].dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
		imageMemoryBarrier[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageMemoryBarrier[0].newLayout = VK_IMAGE_LAYOUT_GENERAL;

		vkCmdPipelineBarrier(commandBuffers[frameIndex], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, imageMemoryBarrier);

		//
		// Update view & projection
		//

		glTF.inverseViewProjection.inverseProjection = glm::inverse(Math::perspective(45.0f, (float)width/(float)height, 0.1f, 100.0f));

		glm::mat3 orbitMatrix = glm::rotate(rotY, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(rotX, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::vec3 orbitEye = orbitMatrix * glm::vec3(0.0f, 0.0f, eyeObjectDistance);

		glTF.inverseViewProjection.inverseView = glm::inverse(glm::lookAt(orbitEye, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

		vkCmdPushConstants(commandBuffers[frameIndex], glTF.scenes[glTF.defaultScene].raytracePipelineLayout, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, 0, sizeof(glTF.inverseViewProjection), &glTF.inverseViewProjection);

		vkCmdPushConstants(commandBuffers[frameIndex], glTF.scenes[glTF.defaultScene].raytracePipelineLayout, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, sizeof(glTF.inverseViewProjection), sizeof(uint32_t), &maxDepth);

		VkStridedBufferRegionKHR rayGenStridedBufferRegion = {};
		rayGenStridedBufferRegion.buffer = glTF.scenes[glTF.defaultScene].shaderBindingBufferResource.buffer;
		rayGenStridedBufferRegion.offset = physicalDeviceRayTracingProperties.shaderGroupHandleSize * 0;
		rayGenStridedBufferRegion.size   = physicalDeviceRayTracingProperties.shaderGroupHandleSize;

		VkStridedBufferRegionKHR missStridedBufferRegion = {};
		missStridedBufferRegion.buffer = glTF.scenes[glTF.defaultScene].shaderBindingBufferResource.buffer;
		missStridedBufferRegion.offset = physicalDeviceRayTracingProperties.shaderGroupHandleSize * 1;
		missStridedBufferRegion.size   = physicalDeviceRayTracingProperties.shaderGroupHandleSize;

		VkStridedBufferRegionKHR closestHitStridedBufferRegion = {};
		closestHitStridedBufferRegion.buffer = glTF.scenes[glTF.defaultScene].shaderBindingBufferResource.buffer;
		closestHitStridedBufferRegion.offset = physicalDeviceRayTracingProperties.shaderGroupHandleSize * 2;
		closestHitStridedBufferRegion.size   = physicalDeviceRayTracingProperties.shaderGroupHandleSize;

		VkStridedBufferRegionKHR callableStridedBufferRegion = {};

		vkCmdBindPipeline(commandBuffers[frameIndex], VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, glTF.scenes[glTF.defaultScene].raytracePipeline);
		vkCmdBindDescriptorSets(commandBuffers[frameIndex], VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, glTF.scenes[glTF.defaultScene].raytracePipelineLayout, 0, 1, &glTF.scenes[glTF.defaultScene].raytraceDescriptorSet, 0, 0);

		vkCmdTraceRaysKHR(commandBuffers[frameIndex], &rayGenStridedBufferRegion, &missStridedBufferRegion, &closestHitStridedBufferRegion, &callableStridedBufferRegion, width, height, 1);

		//
		// Prepare to to copy raytraced image.
		//

		imageMemoryBarrier[0].srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
		imageMemoryBarrier[0].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		imageMemoryBarrier[0].oldLayout = VK_IMAGE_LAYOUT_GENERAL;
		imageMemoryBarrier[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

		imageMemoryBarrier[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		imageMemoryBarrier[1].dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
		imageMemoryBarrier[1].oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		imageMemoryBarrier[1].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

		vkCmdPipelineBarrier(commandBuffers[frameIndex], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, 2, imageMemoryBarrier);

		VkImageCopy imageCopy = {};
		imageCopy.srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
		imageCopy.srcOffset      = {0, 0, 0};
		imageCopy.dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
		imageCopy.dstOffset      = {0, 0, 0};
		imageCopy.extent         = {width, height, 1};
		vkCmdCopyImage(commandBuffers[frameIndex], raytraceImageViewResource.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, swapchainImages[frameIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);

		// Switch back to old layout.s

		imageMemoryBarrier[0].srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
		imageMemoryBarrier[0].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		imageMemoryBarrier[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		imageMemoryBarrier[0].newLayout = VK_IMAGE_LAYOUT_GENERAL;

		imageMemoryBarrier[1].srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
		imageMemoryBarrier[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		imageMemoryBarrier[1].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageMemoryBarrier[1].newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		vkCmdPipelineBarrier(commandBuffers[frameIndex], VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, NULL, 0, NULL, 2, imageMemoryBarrier);
	}
	else
	{
		VkClearColorValue resolveClearColorValue = {};
		resolveClearColorValue.float32[0] = 0.0f;
		resolveClearColorValue.float32[1] = 0.0f;
		resolveClearColorValue.float32[2] = 0.0f;
		resolveClearColorValue.float32[3] = 0.0f;

		VkClearColorValue clearColorValue = {};
		clearColorValue.float32[0] = 0.0f;
		clearColorValue.float32[1] = 0.0f;
		clearColorValue.float32[2] = 0.0f;
		clearColorValue.float32[3] = 1.0f;

		VkClearDepthStencilValue clearDepthStencilValue = {};
		clearDepthStencilValue.depth = 1.0f;

		std::vector<VkClearValue> clearValues;
		if (samples == VK_SAMPLE_COUNT_1_BIT)
		{
			clearValues.resize(2);
			clearValues[0].color = clearColorValue;
			clearValues[1].depthStencil = clearDepthStencilValue;
		}
		else
		{
			clearValues.resize(3);
			clearValues[0].color = resolveClearColorValue;
			clearValues[1].color = clearColorValue;
			clearValues[2].depthStencil = clearDepthStencilValue;
		}

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.framebuffer = framebuffers[frameIndex];
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent = {width, height};
		renderPassBeginInfo.clearValueCount = clearValues.size();
		renderPassBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[frameIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		glTF.viewProjection.projection = Math::perspective(45.0f, (float)width/(float)height, 0.1f, 100.0f);

		glm::mat3 orbitMatrix = glm::rotate(rotY, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(rotX, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::vec3 orbitEye = orbitMatrix * glm::vec3(0.0f, 0.0f, eyeObjectDistance);

		glTF.viewProjection.view = glm::lookAt(orbitEye, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		HelperLoop::draw(glTF, commandBuffers[frameIndex], frameIndex);

		vkCmdEndRenderPass(commandBuffers[frameIndex]);
	}

	//
	// Imgui overlay.
	//

	VkRenderPassBeginInfo imguiRenderPassBeginInfo = {};
	imguiRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	imguiRenderPassBeginInfo.renderPass = imguiRenderPass;
	imguiRenderPassBeginInfo.framebuffer = framebuffers[frameIndex];
	imguiRenderPassBeginInfo.renderArea.offset.x = 0;
	imguiRenderPassBeginInfo.renderArea.offset.y = 0;
	imguiRenderPassBeginInfo.renderArea.extent = {width, height};
	imguiRenderPassBeginInfo.clearValueCount = 0;
	imguiRenderPassBeginInfo.pClearValues = nullptr;

	vkCmdBeginRenderPass(commandBuffers[frameIndex], &imguiRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	// Just render the imgui content.
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffers[frameIndex]);

	vkCmdEndRenderPass(commandBuffers[frameIndex]);

	return true;
}

void Application::applicationTerminate()
{
	Helper::destroyImageViewResource(device, raytraceImageViewResource);

	HelperResource::terminate(glTF, device);
}

// Public

Application::Application(const char* title, const std::string& filename, const std::string& environment) :
	TinyEngine(title), filename(filename), environment(environment)
{
}

Application::~Application()
{
}

void Application::orbitY(float orbit)
{
	rotY -= orbit * 0.01f;
}

void Application::orbitX(float orbit)
{
	rotX -= orbit * 0.01f;

	// Not more than 90 degrees
	rotX = glm::clamp(rotX, -glm::pi<float>() * 0.49f, glm::pi<float>() * 0.49f);
}

void Application::zoom(float zoom)
{
	eyeObjectDistance += zoom * zoomSpeed;

	eyeObjectDistance = glm::clamp(eyeObjectDistance, 0.001f, 1000.0f);
}
