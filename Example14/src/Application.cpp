#include "Application.h"

// Private

bool Application::applicationInit()
{
	renderManager.renderSetupVulkan(physicalDevice, device, queue, commandPool);
	renderManager.renderSetRenderPass(renderPass);
	renderManager.renderSetSamples(samples);
	renderManager.renderSetDimension(width, height);

	//

	renderManager.worldCreate();

	//

	uint64_t lightHandle;
	renderManager.lightCreate(lightHandle);
	renderManager.lightSetEnvironment(lightHandle, environment);
	renderManager.lightFinalize(lightHandle);

	//

	uint64_t cameraHandle;
	renderManager.cameraCreate(cameraHandle);
	renderManager.cameraFinalize(cameraHandle);

	//

	renderManager.worldSetLight(lightHandle);
	renderManager.worldSetCamera(cameraHandle);

	//

	uint64_t positionDataHandle;
	renderManager.sharedDataCreate(positionDataHandle);
	float triangle[] = {
			0.0f, -0.5f, 0.0f,
			0.5f, 0.5f, 0.0f,
			-0.5f, 0.5f, 0.0f
	};
	size_t size = 3 * 3 * sizeof(float);
	renderManager.sharedDataCreateVertexBuffer(positionDataHandle, size, (const void*)triangle);
	renderManager.sharedDataFinalize(positionDataHandle);

	uint64_t colorDataHandle;
	renderManager.sharedDataCreate(colorDataHandle);
	float color[] = {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f
	};
	size = 3 * 3 * sizeof(float);
	renderManager.sharedDataCreateVertexBuffer(colorDataHandle, size, (const void*)color);
	renderManager.sharedDataFinalize(colorDataHandle);

	uint64_t materialHandle;
	renderManager.materialCreate(materialHandle);
	MaterialParameters materialParameters;
	materialParameters.doubleSided = true;
	renderManager.materialSetParameters(materialHandle, materialParameters);
	renderManager.materialFinalize(materialHandle);

	uint64_t geometryHandle;
	renderManager.geometryCreate(geometryHandle);
	renderManager.geometrySetAttribute(geometryHandle, positionDataHandle, "POSITION", 3, VK_FORMAT_R32G32B32_SFLOAT);
	renderManager.geometrySetAttribute(geometryHandle, colorDataHandle, "COLOR_0", 3, VK_FORMAT_R32G32B32_SFLOAT);
	renderManager.geometryFinalize(geometryHandle);

	uint64_t geometryModelHandle;
	renderManager.geometryModelCreate(geometryModelHandle);
	renderManager.geometryModelSetGeometry(geometryModelHandle, geometryHandle);
	renderManager.geometryModelSetMaterial(geometryModelHandle, materialHandle);
	renderManager.geometryModelSetVertexCount(geometryModelHandle, 3);
	renderManager.geometryModelFinalize(geometryModelHandle);

	uint64_t groupHandle;
	renderManager.groupCreate(groupHandle);
	renderManager.groupAddGeometryModel(groupHandle, geometryModelHandle);
	renderManager.groupFinalize(groupHandle);

	uint64_t instanceHandle;
	renderManager.instanceCreate(instanceHandle);
	renderManager.instanceSetGroup(instanceHandle, groupHandle);
	renderManager.instanceFinalize(instanceHandle);

	renderManager.worldAddInstance(instanceHandle);

	//

	renderManager.worldFinalize();

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

	glm::mat4 viewMatrix = glm::lookAt(orbitEye, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	renderManager.cameraUpdateProjectionMatrix(cameraHandle, projectionMatrix);
	renderManager.cameraUpdateViewMatrix(cameraHandle, viewMatrix);

	renderManager.rasterize(commandBuffers[frameIndex], frameIndex, ALL);

	vkCmdEndRenderPass(commandBuffers[frameIndex]);

	return true;
}

void Application::applicationTerminate()
{
	renderManager.terminate();
}

// Public

Application::Application(const std::string& environment) :
	environment(environment)
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
	eyeObjectDistance += zoom * 0.05f;

	eyeObjectDistance = glm::clamp(eyeObjectDistance, 0.001f, 1000.0f);
}
