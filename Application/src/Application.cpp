#include "Application.h"

// Private

bool Application::applicationInit()
{
	renderManager.renderSetupVulkan(physicalDevice, device, queue, commandPool);
	renderManager.renderSetRenderPass(renderPass);
	renderManager.renderSetSamples(samples);
	renderManager.renderSetDimension(width, height);
	renderManager.renderSetFrames(swapchainImages.size());

	HelperLoad helperLoad;
	if(!helperLoad.open(glTF, filename))
	{
		return false;
	}

	if (!HelperUpdate::update(glTF, glm::mat4(1.0f)))
	{
		return false;
	}

	WorldBuilder worldBuilder(glTF, environment, renderManager);
	if(!worldBuilder.build())
	{
		return false;
	}

	nodeToHandles = worldBuilder.cloneNodeToHandles();

	//

	float stop = 0.0f;
	if (HelperAnimate::gatherStop(stop, glTF, 0))
	{
		animationController.setStopTime(stop);
		animationController.setPlay(true);
	}
	else
	{
		animationController.setPlay(false);
		animate = false;
	}

	return true;
}

bool Application::applicationUpdate(uint32_t frameIndex, double deltaTime, double totalTime)
{
	uint64_t cameraHandle = 0;

	if (!renderManager.worldGetCamera(cameraHandle))
	{
		return false;
	}

	//

	ImGui_ImplVulkan_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Settings");

	focused = ImGui::IsWindowFocused();

	if (animationController.isPlay())
	{
		ImGui::Checkbox("Animate", &animate);
		ImGui::Separator();
	}
	ImGui::SliderFloat("World Scale", &worldScale, 0.1f, 10.0f, "ratio = %.1f");
	ImGui::Separator();
	ImGui::SliderFloat("Zoom Speed", &zoomSpeed, 0.01f, 0.1f, "ratio = %.2f");
	ImGui::End();

	ImGui::Render();

	//

	if (animate)
	{
		animationController.updateCurrentTime(deltaTime);
		HelperAnimate::update(glTF, 0, animationController.getCurrentTime());
	}
	HelperUpdate::update(glTF, glm::scale(glm::vec3(worldScale)));

	//

	// Update the animations to the renderer.
	for (size_t i = 0; i < glTF.nodes.size(); i++)
	{
		const Node& node = glTF.nodes[i];
		renderManager.instanceUpdateWorldMatrix(nodeToHandles[&node], node.worldMatrix);

		if (node.weights.size() > 0 && node.mesh >= 0)
		{
			renderManager.instanceUpdateWeights(nodeToHandles[&node], node.weights, frameIndex);
		}
		if (node.jointMatrices.size() > 0 && node.mesh >= 0)
		{
			renderManager.instanceUpdateJointMatrices(nodeToHandles[&node], node.jointMatrices, frameIndex);
		}
	}

	//

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
	renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassBeginInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffers[frameIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	glm::mat4 projectionMatrix = Projection::perspective(45.0f, (float)width/(float)height, 0.1f, 100.0f);

	glm::mat3 orbitMatrix = glm::rotate(rotY, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(rotX, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::vec3 orbitEye = orbitMatrix * glm::vec3(0.0f, 0.0f, eyeObjectDistance);
	glm::vec3 orbitCenter = orbitMatrix * glm::vec3(0.0f, 0.0f, 0.0f);

	glm::mat4 viewMatrix = glm::translate(glm::vec3(posX, posY, 0.0f)) * glm::lookAt(orbitEye, orbitCenter, glm::vec3(0.0f, 1.0f, 0.0f));

	renderManager.cameraUpdateProjectionMatrix(cameraHandle, projectionMatrix);
	renderManager.cameraUpdateViewMatrix(cameraHandle, viewMatrix);

	renderManager.draw(commandBuffers[frameIndex], frameIndex, OPAQUE);
	renderManager.draw(commandBuffers[frameIndex], frameIndex, TRANSPARENT);

	//

	// Render the imgui content.
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffers[frameIndex]);

	vkCmdEndRenderPass(commandBuffers[frameIndex]);

	return true;
}

void Application::applicationTerminate()
{
	renderManager.terminate();
}

// Public

Application::Application(const std::string& filename, const std::string& environment) :
	filename(filename), environment(environment)
{
}

Application::~Application()
{
}

void Application::orbitY(float orbit)
{
	if (!focused)
	{
		rotY -= orbit * 0.01f;
	}
}

void Application::orbitX(float orbit)
{
	if (!focused)
	{
		rotX -= orbit * 0.01f;

		// Not more than 90 degrees
		rotX = glm::clamp(rotX, -glm::pi<float>() * 0.49f, glm::pi<float>() * 0.49f);
	}
}

void Application::panX(float deltaX)
{
	if (!focused)
	{
		posX += deltaX * 0.05f;
	}
}

void Application::panY(float deltaY)
{
	if (!focused)
	{
		posY -= deltaY * 0.05f;
	}
}

void Application::zoom(float zoom)
{
	if (!focused)
	{
		eyeObjectDistance += zoom * 0.05f;

		eyeObjectDistance = glm::clamp(eyeObjectDistance, 0.001f, 1000.0f);
	}
}
