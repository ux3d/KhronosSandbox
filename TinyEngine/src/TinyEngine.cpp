#include "TinyEngine.h"

#include <chrono>

// Private

bool TinyEngine::createInstance()
{
	if (inResize)
	{
		return true;
	}

	VkResult result = VK_SUCCESS;

	//

	result = volkInitialize();
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	//

	VkApplicationInfo applicationInfo = {};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pApplicationName = applicationName.c_str();
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.pEngineName = "Tiny Engine";
	applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.apiVersion = VK_MAKE_VERSION(major, minor, patch);

	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &applicationInfo;
	instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(enabledInstanceLayerNames.size());
	instanceCreateInfo.ppEnabledLayerNames = enabledInstanceLayerNames.data();
	instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledInstanceExtensionNames.size());
	instanceCreateInfo.ppEnabledExtensionNames = enabledInstanceExtensionNames.data();

	//

	for (const char* instanceLayerName : enabledInstanceLayerNames)
	{
		Logger::print(TinyEngine_INFO, __FILE__, __LINE__, "Instance layer '%s'", instanceLayerName);
	}
	for (const char* instanceExtensionNames : enabledInstanceExtensionNames)
	{
		Logger::print(TinyEngine_INFO, __FILE__, __LINE__, "Instance extension '%s'", instanceExtensionNames);
	}

	//

	result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	//

	volkLoadInstance(instance);

	Logger::print(TinyEngine_INFO, __FILE__, __LINE__, "Created VkInstance with version %u.%u.%u", major, minor, patch);

	return true;
}

bool TinyEngine::choosePhysicalDevice()
{
	if (inResize)
	{
		return true;
	}

	VkResult result = VK_SUCCESS;

	uint32_t physicalDeviceCount = 0;
	result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}
	if (physicalDeviceCount == 0)
	{
		return false;
	}

	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	for (const VkPhysicalDevice& currentPhysicalDevice : physicalDevices)
	{
		VkPhysicalDeviceProperties physicalDeviceProperties = {};
		vkGetPhysicalDeviceProperties(currentPhysicalDevice, &physicalDeviceProperties);

		if (samples != VK_SAMPLE_COUNT_1_BIT)
		{
			VkSampleCountFlags counts = (physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts);

			if ((counts & samples) != samples)
			{
				continue;
			}
		}

		VkPhysicalDeviceFeatures physicalDeviceFeatures = {};
		vkGetPhysicalDeviceFeatures(currentPhysicalDevice, &physicalDeviceFeatures);

		uint32_t propertyCount = 0;
		result = vkEnumerateDeviceExtensionProperties(currentPhysicalDevice, nullptr, &propertyCount, nullptr);
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}
		std::vector<VkExtensionProperties> extensionProperties(propertyCount);
		result = vkEnumerateDeviceExtensionProperties(currentPhysicalDevice, nullptr, &propertyCount, extensionProperties.data());
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}

		bool hasSwapchain = false;
		for (const VkExtensionProperties& currentExtensionProperties : extensionProperties)
		{
			if (strcmp(currentExtensionProperties.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
			{
				hasSwapchain = true;
				break;
			}
		}
		if (!hasSwapchain)
		{
			continue;
		}

		if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			physicalDevice = currentPhysicalDevice;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE)
	{
		return false;
	}

	return true;
}

bool TinyEngine::createSurface()
{
	if (inResize)
	{
		return true;
	}

	VkResult result = VK_SUCCESS;

	VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
	result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}
	uint32_t imageCount = 0;
	if (surfaceCapabilities.maxImageCount != 0)
	{
		imageCount = glm::min(surfaceCapabilities.maxImageCount, 3u);
	}
	else
	{
		imageCount = 3;
	}
	if (imageCount == 0)
	{
		return false;
	}
	minImageCount = imageCount;

	uint32_t surfaceFormatCount = 0;
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}
	if (surfaceFormatCount == 0)
	{
		return false;
	}

	std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats.data());
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	for (const VkSurfaceFormatKHR& currentSurfaceFormat : surfaceFormats)
	{
		if (currentSurfaceFormat.format == colorFormat)
		{
			surfaceFormat = currentSurfaceFormat;
			break;
		}
	}
	if (surfaceFormat.format == VK_FORMAT_UNDEFINED)
	{
		return false;
	}

	uint32_t presentModeCount;
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}
	if (presentModeCount == 0)
	{
		return false;
	}

	std::vector<VkPresentModeKHR> presentModes(presentModeCount);
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	for (const VkPresentModeKHR& currentPresentMode : presentModes)
	{
		if (currentPresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			presentMode = currentPresentMode;
			break;
		}
	}

	return true;
}

bool TinyEngine::createDevice()
{
	if (inResize)
	{
		return true;
	}

	VkResult result = VK_SUCCESS;

	uint32_t queueFamilyPropertyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties.data());

	for (uint32_t currentQueueFamilyIndex = 0; currentQueueFamilyIndex < queueFamilyPropertyCount; currentQueueFamilyIndex++)
	{
		VkBool32 surfaceSupport = VK_FALSE;
		result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, currentQueueFamilyIndex, surface, &surfaceSupport);
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}

		if ((queueFamilyProperties[currentQueueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) && surfaceSupport)
		{
			queueFamilyIndex = currentQueueFamilyIndex;
			break;
		}
	}
	if (!queueFamilyIndex.has_value())
	{
		return false;
	}

	float queuePriorities = 1.0f;

	VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
	deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndex.value();
	deviceQueueCreateInfo.queueCount = 1;
	deviceQueueCreateInfo.pQueuePriorities = &queuePriorities;

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledDeviceExtensionNames.size());
	deviceCreateInfo.ppEnabledExtensionNames = enabledDeviceExtensionNames.data();

	VkPhysicalDeviceFeatures physicalDeviceFeatures = {};

	VkPhysicalDeviceFeatures2 physicalDeviceFeatures2 = {};
	physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	VkPhysicalDeviceBufferDeviceAddressFeatures physicalDeviceBufferDeviceAddressFeatures = {};
	physicalDeviceBufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	VkPhysicalDeviceDescriptorIndexingFeatures physicalDeviceDescriptorIndexingFeatures = {};
	physicalDeviceDescriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
	VkPhysicalDeviceIndexTypeUint8FeaturesEXT physicalDeviceIndexTypeUint8Features = {};
	physicalDeviceIndexTypeUint8Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES_EXT;

	if (major == 1 && minor < 1)
	{
		deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
	}
	else
	{
		void** ppNext = &physicalDeviceFeatures2.pNext;

		if (hasEnabledDeviceExtensionName(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME))
		{
			physicalDeviceBufferDeviceAddressFeatures.bufferDeviceAddress = VK_TRUE;

			*ppNext = &physicalDeviceBufferDeviceAddressFeatures;
			ppNext = &physicalDeviceBufferDeviceAddressFeatures.pNext;
		}
		if (hasEnabledDeviceExtensionName(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME))
		{
			physicalDeviceDescriptorIndexingFeatures.runtimeDescriptorArray = VK_TRUE;

			*ppNext = &physicalDeviceDescriptorIndexingFeatures;
			ppNext = &physicalDeviceDescriptorIndexingFeatures.pNext;
		}
		if (hasEnabledDeviceExtensionName(VK_EXT_INDEX_TYPE_UINT8_EXTENSION_NAME))
		{
			physicalDeviceIndexTypeUint8Features.indexTypeUint8 = VK_TRUE;

			*ppNext = &physicalDeviceIndexTypeUint8Features;
			ppNext = &physicalDeviceIndexTypeUint8Features.pNext;
		}

		deviceCreateInfo.pNext = &physicalDeviceFeatures2;
	}

	//

	for (const char* deviceExtensionName : enabledDeviceExtensionNames)
	{
		Logger::print(TinyEngine_INFO, __FILE__, __LINE__, "Device extension '%s'", deviceExtensionName);
	}

	//

	result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	vkGetDeviceQueue(device, queueFamilyIndex.value(), 0, &queue);

	//

	volkLoadDevice(device);

	//

	Logger::print(TinyEngine_INFO, __FILE__, __LINE__, "Created VkDevice");

	return true;
}

bool TinyEngine::createSwapchain()
{
	VkResult result = VK_SUCCESS;

	//

	VkSurfaceCapabilitiesKHR surfaceCapabilities = {};

	result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	width = surfaceCapabilities.currentExtent.width;
	height = surfaceCapabilities.currentExtent.height;

	//

	VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.surface = surface;
	swapchainCreateInfo.minImageCount = minImageCount;
	swapchainCreateInfo.imageFormat = surfaceFormat.format;
	swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapchainCreateInfo.imageExtent = {width, height};
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage = imageUsage;
	swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfo.presentMode = presentMode;
	swapchainCreateInfo.clipped = VK_TRUE;

	result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	uint32_t swapchainImageCount = 0;
	result = vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}
	swapchainImages.resize(minImageCount, VK_NULL_HANDLE);
	if (swapchainImageCount != swapchainImages.size())
	{
		return false;
	}

	result = vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages.data());
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	swapchainImageViews.resize(swapchainImages.size(), VK_NULL_HANDLE);

	for (size_t i = 0; i < swapchainImages.size(); i++)
	{
		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = swapchainImages[i];
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = surfaceFormat.format;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &swapchainImageViews[i]);
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}
	}

	frameIndex = 0;

	return true;
}

bool TinyEngine::createColorResources()
{
	if (samples == VK_SAMPLE_COUNT_1_BIT)
	{
		return true;
	}

	ImageViewResourceCreateInfo imageViewResourceCreateInfo = {};
	imageViewResourceCreateInfo.format = surfaceFormat.format;
	imageViewResourceCreateInfo.extent = {width, height, 1};
	imageViewResourceCreateInfo.mipLevels = 1;
	imageViewResourceCreateInfo.samples = samples;
	imageViewResourceCreateInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	imageViewResourceCreateInfo.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

	return VulkanResource::createImageViewResource(physicalDevice, device, msaa, imageViewResourceCreateInfo);
}

bool TinyEngine::createDepthStencilResources()
{
	if (depthStencilFormat == VK_FORMAT_UNDEFINED)
	{
		return true;
	}

	if (!HelperVulkan::isDepthStencilFormat(depthStencilFormat))
	{
		return false;
	}

	ImageViewResourceCreateInfo imageViewResourceCreateInfo = {};
	imageViewResourceCreateInfo.format = depthStencilFormat;
	imageViewResourceCreateInfo.extent = {width, height, 1};
	imageViewResourceCreateInfo.mipLevels = 1;
	imageViewResourceCreateInfo.samples = samples;
	imageViewResourceCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageViewResourceCreateInfo.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

	return VulkanResource::createImageViewResource(physicalDevice, device, depth, imageViewResourceCreateInfo);
}

bool TinyEngine::createRenderpass()
{
	VkResult result = VK_SUCCESS;

	uint32_t colorAttachmentIndex = 0;
	uint32_t msaaAttachmentIndex = 0;
	uint32_t depthAttachmentIndex = 0;

	uint32_t attachmentCount = 1;
	if (samples != VK_SAMPLE_COUNT_1_BIT)
	{
		msaaAttachmentIndex = attachmentCount;
		attachmentCount++;
	}
	if (depthStencilFormat != VK_FORMAT_UNDEFINED)
	{
		depthAttachmentIndex = attachmentCount;
		attachmentCount++;
	}

	std::vector<VkAttachmentReference> attachmentReference(attachmentCount);

	attachmentReference[colorAttachmentIndex].attachment = colorAttachmentIndex;
	attachmentReference[colorAttachmentIndex].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	if (samples != VK_SAMPLE_COUNT_1_BIT)
	{
		attachmentReference[msaaAttachmentIndex].attachment = msaaAttachmentIndex;
		attachmentReference[msaaAttachmentIndex].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}
	if (depthStencilFormat != VK_FORMAT_UNDEFINED)
	{
		attachmentReference[depthAttachmentIndex].attachment = depthAttachmentIndex;
		attachmentReference[depthAttachmentIndex].layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}

	VkSubpassDescription subpassDescription = {};

	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &attachmentReference[colorAttachmentIndex];
	if (samples != VK_SAMPLE_COUNT_1_BIT)
	{
		subpassDescription.pColorAttachments = &attachmentReference[msaaAttachmentIndex];
		subpassDescription.pResolveAttachments = &attachmentReference[colorAttachmentIndex];
	}
	if (depthStencilFormat != VK_FORMAT_UNDEFINED)
	{
		subpassDescription.pDepthStencilAttachment = &attachmentReference[depthAttachmentIndex];
	}

	std::vector<VkAttachmentDescription> attachmentDescription(attachmentCount);

	attachmentDescription[colorAttachmentIndex].format = surfaceFormat.format;
	attachmentDescription[colorAttachmentIndex].samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescription[colorAttachmentIndex].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescription[colorAttachmentIndex].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescription[colorAttachmentIndex].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription[colorAttachmentIndex].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription[colorAttachmentIndex].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	if (samples != VK_SAMPLE_COUNT_1_BIT)
	{
		attachmentDescription[msaaAttachmentIndex].format = surfaceFormat.format;
		attachmentDescription[msaaAttachmentIndex].samples = samples;
		attachmentDescription[msaaAttachmentIndex].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescription[msaaAttachmentIndex].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescription[msaaAttachmentIndex].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescription[msaaAttachmentIndex].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescription[msaaAttachmentIndex].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}
	if (depthStencilFormat != VK_FORMAT_UNDEFINED)
	{
		attachmentDescription[depthAttachmentIndex].format = depthStencilFormat;
		attachmentDescription[depthAttachmentIndex].samples = samples;
		attachmentDescription[depthAttachmentIndex].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescription[depthAttachmentIndex].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescription[depthAttachmentIndex].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescription[depthAttachmentIndex].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescription[depthAttachmentIndex].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}

	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = attachmentCount;
	renderPassCreateInfo.pAttachments = attachmentDescription.data();
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpassDescription;

	result = vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	if (useImgui)
	{
		for (VkAttachmentDescription& currentAttachmentDescription : attachmentDescription)
		{
			currentAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		}

		result = vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &imguiRenderPass);
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}
	}

	return true;
}

bool TinyEngine::createFramebuffer()
{
	VkResult result = VK_SUCCESS;

	framebuffers.resize(swapchainImages.size(), VK_NULL_HANDLE);

	uint32_t attachmentCount = 1;
	if (samples != VK_SAMPLE_COUNT_1_BIT)
	{
		attachmentCount++;
	}
	if (depthStencilFormat != VK_FORMAT_UNDEFINED)
	{
		attachmentCount++;
	}

	VkFramebufferCreateInfo framebufferCreateInfo = {};
	framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferCreateInfo.renderPass = renderPass;
	framebufferCreateInfo.attachmentCount = attachmentCount;
	framebufferCreateInfo.width = width;
	framebufferCreateInfo.height = height;
	framebufferCreateInfo.layers = 1;

	std::vector<VkImageView> attachments(attachmentCount);

	for (size_t i = 0; i < framebuffers.size(); i++)
	{
		attachments.clear();

		uint32_t attachmentIndex = 0;
		attachments[attachmentIndex] = swapchainImageViews[i];
		if (samples != VK_SAMPLE_COUNT_1_BIT)
		{
			attachmentIndex++;
			attachments[attachmentIndex] = msaa.imageView;
		}
		if (depthStencilFormat != VK_FORMAT_UNDEFINED)
		{
			attachmentIndex++;
			attachments[attachmentIndex] = depth.imageView;
		}

		framebufferCreateInfo.pAttachments = attachments.data();

		result = vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffers[i]);
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}
	}

	return true;
}

bool TinyEngine::createCommandResources()
{
	if (inResize)
	{
		return true;
	}

	VkResult result = VK_SUCCESS;

	VkCommandPoolCreateInfo commandPoolCreateInfo = {};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex.value();

	result = vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	commandBuffers.resize(swapchainImages.size(), VK_NULL_HANDLE);

	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.commandPool = commandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers.data());
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	return true;
}

bool TinyEngine::createSynchronizationResources()
{
	if (inResize)
	{
		return true;
	}

	VkResult result = VK_SUCCESS;

	imageAvailableSemaphores.resize(swapchainImages.size(), VK_NULL_HANDLE);
	renderFinishedSemaphores.resize(swapchainImages.size(), VK_NULL_HANDLE);
	queueSubmitFences.resize(swapchainImages.size(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0;  i < swapchainImages.size(); i++)
	{
		result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]);
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}

		result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]);
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}

		//

		result = vkCreateFence(device, &fenceCreateInfo, nullptr, &queueSubmitFences[i]);
		if (result != VK_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

			return false;
		}
	}

	return true;
}

bool TinyEngine::createImgui()
{
	if (!useImgui)
	{
		return true;
	}

	VkResult result = VK_SUCCESS;

	//

	const uint32_t BufferSize = 1000;
	VkDescriptorPoolSize descriptorPoolSizes[] = {
		{ VK_DESCRIPTOR_TYPE_SAMPLER, BufferSize },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, BufferSize },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, BufferSize },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, BufferSize },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, BufferSize },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, BufferSize },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, BufferSize },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, BufferSize },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, BufferSize },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, BufferSize },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, BufferSize }
	};

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	descriptorPoolCreateInfo.maxSets = BufferSize * IM_ARRAYSIZE(descriptorPoolSizes);
	descriptorPoolCreateInfo.poolSizeCount = (uint32_t)IM_ARRAYSIZE(descriptorPoolSizes);
	descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes;
	result = vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &imguiDescriptorPool);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	ImGui_ImplVulkan_InitInfo initInfo = {};
	initInfo.Instance = instance;
	initInfo.PhysicalDevice = physicalDevice;
	initInfo.Device = device;
	initInfo.QueueFamily = queueFamilyIndex.value();
	initInfo.Queue = queue;
	initInfo.DescriptorPool = imguiDescriptorPool;
	initInfo.MinImageCount = 2;
	initInfo.ImageCount = static_cast<uint32_t>(swapchainImageViews.size());
	initInfo.MSAASamples = samples;
	if (!ImGui_ImplVulkan_Init(&initInfo, renderPass))
	{
		return false;
	}

	VkCommandBuffer imguiCommandBuffer = VK_NULL_HANDLE;
	if (!HelperVulkan::beginOneTimeSubmitCommand(device, commandPool, imguiCommandBuffer))
	{
		return false;
	}

	if (!ImGui_ImplVulkan_CreateFontsTexture(imguiCommandBuffer))
	{
		return false;
	}

	if (!HelperVulkan::endOneTimeSubmitCommand(device, queue, commandPool, imguiCommandBuffer))
	{
		return false;
	}

	return true;
}

bool TinyEngine::createConfiguration()
{
	if (inResize)
	{
		return true;
	}

	vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
	vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);

	if (major >= 1)
	{
		if (minor >= 1)
		{
			physicalDeviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
			physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

			vkGetPhysicalDeviceProperties2(physicalDevice, &physicalDeviceProperties2);
			vkGetPhysicalDeviceFeatures2(physicalDevice, &physicalDeviceFeatures2);
		}
	}
	else
	{
		return false;
	}

	return true;
}

// Public

TinyEngine::TinyEngine()
{
}

TinyEngine::~TinyEngine()
{
}

bool TinyEngine::prepare()
{
	if (!createInstance())
	{
		return false;
	}

	return true;
}

bool TinyEngine::init(VkSurfaceKHR surface)
{
	if (!surface)
	{
		return false;
	}

	this->surface = surface;

	if (!choosePhysicalDevice())
	{
		return false;
	}

	if (!createSurface())
	{
		return false;
	}

	if (!createDevice())
	{
		return false;
	}

	if (!createSwapchain())
	{
		return false;
	}

	if (!createColorResources())
	{
		return false;
	}

	if (!createDepthStencilResources())
	{
		return false;
	}

	if (!createRenderpass())
	{
		return false;
	}

	if (!createFramebuffer())
	{
		return false;
	}

	if (!createCommandResources())
	{
		return false;
	}

	if (!createSynchronizationResources())
	{
		return false;
	}

	if (!createImgui())
	{
		return false;
	}

	if (!createConfiguration())
	{
		return false;
	}

	if (useOpenXR)
	{
		if (!xrEngine.init(instance, physicalDevice, device, queueFamilyIndex.value(), 0, surfaceFormat.format))
		{
			return false;
		}
	}

	return applicationInit();
}

bool TinyEngine::resize()
{
	inResize = true;

	return true;
}

bool TinyEngine::update()
{
	if (inResize)
	{
		if (!terminate())
		{
			// Switch back to clean up later everything.
			inResize = false;

			return false;
		}

		if (!init(surface))
		{
			// Switch back to clean up later everything.
			inResize = false;

			return false;
		}

		inResize = false;

		return true;
	}

	//

	static std::optional<double> firstTime;
	static double lastTime = 0.0;

	double currentTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() / 1000000.0;

	if (!firstTime.has_value())
	{
		firstTime = currentTime;
		lastTime = currentTime;
	}

	double deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	//

	VkResult result = VK_SUCCESS;

	result = vkWaitForFences(device, 1, &queueSubmitFences[frameIndex], VK_TRUE, UINT64_MAX);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	result = vkResetFences(device, 1, &queueSubmitFences[frameIndex]);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemaphores[frameIndex], VK_NULL_HANDLE, &frameIndex);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	//

	VkCommandBufferBeginInfo commandBufferBeginInfo = {};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	result = vkBeginCommandBuffer(commandBuffers[frameIndex], &commandBufferBeginInfo);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	uint32_t imageMemoryBarrierCount = 1;
	if (depthStencilFormat != VK_FORMAT_UNDEFINED)
	{
		imageMemoryBarrierCount++;
	}
	std::vector<VkImageMemoryBarrier> imageMemoryBarrier(imageMemoryBarrierCount);

	imageMemoryBarrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageMemoryBarrier[0].subresourceRange.levelCount = 1;
	imageMemoryBarrier[0].subresourceRange.layerCount = 1;
	imageMemoryBarrier[0].image = swapchainImages[frameIndex];

	imageMemoryBarrier[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	imageMemoryBarrier[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
	imageMemoryBarrier[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageMemoryBarrier[0].newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	if (depthStencilFormat != VK_FORMAT_UNDEFINED)
	{
		imageMemoryBarrier[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier[1].subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		imageMemoryBarrier[1].subresourceRange.levelCount = 1;
		imageMemoryBarrier[1].subresourceRange.layerCount = 1;
		imageMemoryBarrier[1].image = depth.image;

		imageMemoryBarrier[1].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		imageMemoryBarrier[1].dstAccessMask = 0;
		imageMemoryBarrier[1].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageMemoryBarrier[1].newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}

	vkCmdPipelineBarrier(commandBuffers[frameIndex], VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, NULL, 0, NULL, imageMemoryBarrierCount, imageMemoryBarrier.data());

	//

	if (useOpenXR)
	{
		if (!xrEngine.update())
		{
			return false;
		}
	}

	if (!applicationUpdate(frameIndex, deltaTime, currentTime - firstTime.value()))
	{
		return false;
	}

	//

	imageMemoryBarrier[0].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
	imageMemoryBarrier[0].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	imageMemoryBarrier[0].oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	imageMemoryBarrier[0].newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	if (depthStencilFormat != VK_FORMAT_UNDEFINED)
	{
		imageMemoryBarrier[1].srcAccessMask = 0;
		imageMemoryBarrier[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		imageMemoryBarrier[1].oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		imageMemoryBarrier[1].newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
	}

	vkCmdPipelineBarrier(commandBuffers[frameIndex], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, imageMemoryBarrierCount, imageMemoryBarrier.data());

	result = vkEndCommandBuffer(commandBuffers[frameIndex]);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	// Submit and ...
	VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[frameIndex]};
	VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[frameIndex]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[frameIndex];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	result = vkQueueSubmit(queue, 1, &submitInfo, queueSubmitFences[frameIndex]);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	// ... and present.

	VkSwapchainKHR swapchains[] = {swapchain};

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &frameIndex;

	result = vkQueuePresentKHR(queue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		Logger::print(TinyEngine_VERBOSE, __FILE__, __LINE__, result);

		inResize = true;

		return true;
	}
	else if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

		return false;
	}

	frameIndex = (frameIndex + 1) % framebuffers.size();

	return true;
}

bool TinyEngine::terminate()
{
	if (useOpenXR)
	{
		xrEngine.terminate();
	}

	//

	if (device)
	{
		vkDeviceWaitIdle(device);
	}

	//

	if (useImgui)
	{
		ImGui_ImplVulkan_Shutdown();

		if (imguiDescriptorPool != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorPool(device, imguiDescriptorPool, nullptr);

			imguiDescriptorPool = VK_NULL_HANDLE;
		}
	}

	//

	applicationTerminate();

	if (!inResize)
	{
		physicalDeviceProperties = {};
		physicalDeviceProperties2 = {};

		physicalDeviceFeatures = {};
		physicalDeviceFeatures2 = {};

		for (const VkFence& currentFence : queueSubmitFences)
		{
			if (currentFence != VK_NULL_HANDLE)
			{
				 vkDestroyFence(device, currentFence, nullptr);
			}
		}
		queueSubmitFences.clear();

		for (const VkSemaphore& currentSemaphore : imageAvailableSemaphores)
		{
			if (currentSemaphore != VK_NULL_HANDLE)
			{
				 vkDestroySemaphore(device, currentSemaphore, nullptr);
			}
		}
		imageAvailableSemaphores.clear();

		for (const VkSemaphore& currentSemaphore : renderFinishedSemaphores)
		{
			if (currentSemaphore != VK_NULL_HANDLE)
			{
				 vkDestroySemaphore(device, currentSemaphore, nullptr);
			}
		}
		renderFinishedSemaphores.clear();

		commandBuffers.clear();

		if (commandPool != VK_NULL_HANDLE)
		{
			vkDestroyCommandPool(device, commandPool, nullptr);
			commandPool = VK_NULL_HANDLE;
		}
	}

	for (const VkFramebuffer& currentFramebuffer : framebuffers)
	{
		if (currentFramebuffer != VK_NULL_HANDLE)
		{
			 vkDestroyFramebuffer(device, currentFramebuffer, nullptr);
		}
	}
	framebuffers.clear();

	if (renderPass != VK_NULL_HANDLE)
	{
		vkDestroyRenderPass(device, renderPass, nullptr);
		renderPass = VK_NULL_HANDLE;
	}

	if (useImgui)
	{
		if (imguiRenderPass != VK_NULL_HANDLE)
		{
			vkDestroyRenderPass(device, imguiRenderPass, nullptr);
			imguiRenderPass = VK_NULL_HANDLE;
		}
	}

	VulkanResource::destroyImageViewResource(device, depth);

	VulkanResource::destroyImageViewResource(device, msaa);

	for (const VkImageView& currentImageView : swapchainImageViews)
	{
		if (currentImageView != VK_NULL_HANDLE)
		{
			 vkDestroyImageView(device, currentImageView, nullptr);
		}
	}
	swapchainImageViews.clear();

	swapchainImages.clear();

	if (swapchain)
	{
		vkDestroySwapchainKHR(device, swapchain, nullptr);
		swapchain = VK_NULL_HANDLE;
	}
	frameIndex = 0;

	if (!inResize)
	{
		queue = VK_NULL_HANDLE;
		queueFamilyIndex.reset();

		if (device)
		{
			vkDestroyDevice(device, nullptr);
			device = VK_NULL_HANDLE;
		}

		surfaceFormat = {};
		presentMode = VK_PRESENT_MODE_FIFO_KHR;
		minImageCount = 0;

		if (surface)
		{
			vkDestroySurfaceKHR(instance, surface, nullptr);
			surface = nullptr;
		}

		physicalDevice = VK_NULL_HANDLE;

		if (instance)
		{
			vkDestroyInstance(instance, nullptr);
			instance = VK_NULL_HANDLE;
		}

		width = 0;
		height = 0;
	}

	return true;
}

VkInstance TinyEngine::getInstance() const
{
	return instance;
}

const std::string& TinyEngine::getApplicationName() const
{
	return applicationName;
}

void TinyEngine::setApplicationName(const std::string& applicationName)
{
	this->applicationName = applicationName;
}

uint32_t TinyEngine::getMajor() const
{
	return major;
}

void TinyEngine::setMajor(uint32_t major)
{
	this->major = major;
}

uint32_t TinyEngine::getMinor() const
{
	return minor;
}

void TinyEngine::setMinor(uint32_t minor)
{
	this->minor = minor;
}

uint32_t TinyEngine::getPatch() const
{
	return patch;
}

void TinyEngine::setPatch(uint32_t patch)
{
	this->patch = patch;
}

void TinyEngine::addEnabledInstanceLayerName(const char* layerName)
{
	for (auto it = enabledInstanceLayerNames.begin(); it != enabledInstanceLayerNames.end(); it++)
	{
		if (strcmp(*it, layerName) == 0)
		{
			return;
		}
	}

	enabledInstanceLayerNames.push_back(layerName);
}

void TinyEngine::removeEnabledInstanceLayerName(const char* layerName)
{
	for (auto it = enabledInstanceLayerNames.begin(); it != enabledInstanceLayerNames.end(); it++)
	{
		if (strcmp(*it, layerName) == 0)
		{
			enabledInstanceLayerNames.erase(it);
			return;
		}
	}
}

bool TinyEngine::hasEnabledInstanceExtensionName(const char* extensionName) const
{
	for (auto it = enabledInstanceExtensionNames.begin(); it != enabledInstanceExtensionNames.end(); it++)
	{
		if (strcmp(*it, extensionName) == 0)
		{
			return true;
		}
	}

	return false;
}

void TinyEngine::addEnabledInstanceExtensionName(const char* extensionName)
{
	if (hasEnabledInstanceExtensionName(extensionName))
	{
		return;
	}

	enabledInstanceExtensionNames.push_back(extensionName);
}

void TinyEngine::removeEnabledInstanceExtensionName(const char* extensionName)
{
	for (auto it = enabledInstanceExtensionNames.begin(); it != enabledInstanceExtensionNames.end(); it++)
	{
		if (strcmp(*it, extensionName) == 0)
		{
			enabledInstanceExtensionNames.erase(it);
			return;
		}
	}
}

bool TinyEngine::hasEnabledDeviceExtensionName(const char* extensionName) const
{
	for (auto it = enabledDeviceExtensionNames.begin(); it != enabledDeviceExtensionNames.end(); it++)
	{
		if (strcmp(*it, extensionName) == 0)
		{
			return true;
		}
	}

	return false;;
}

void TinyEngine::addEnabledDeviceExtensionName(const char* extensionName)
{
	if (hasEnabledDeviceExtensionName(extensionName))
	{
		return;
	}

	enabledDeviceExtensionNames.push_back(extensionName);
}

void TinyEngine::removeEnabledDeviceExtensionName(const char* extensionName)
{
	for (auto it = enabledDeviceExtensionNames.begin(); it != enabledDeviceExtensionNames.end(); it++)
	{
		if (strcmp(*it, extensionName) == 0)
		{
			enabledDeviceExtensionNames.erase(it);
			return;
		}
	}
}

VkFormat TinyEngine::getColorFormat() const
{
	return colorFormat;
}

void TinyEngine::setColorFormat(VkFormat colorFormat)
{
	this->colorFormat = colorFormat;
}

VkFormat TinyEngine::getDepthStencilFormat() const
{
	return depthStencilFormat;
}

void TinyEngine::setDepthStencilFormat(VkFormat depthFormat)
{
	this->depthStencilFormat = depthFormat;
}

VkSampleCountFlagBits TinyEngine::getSamples() const
{
	return samples;
}

void TinyEngine::setSamples(VkSampleCountFlagBits samples)
{
	this->samples = samples;
}

VkImageUsageFlags TinyEngine::getImageUsage() const
{
	return imageUsage;
}

void TinyEngine::setImageUsage(VkImageUsageFlags imageUsage)
{
	this->imageUsage = imageUsage;
}

bool TinyEngine::isUseImgui() const
{
	return useImgui;
}

void TinyEngine::setUseImgui(bool useImgui)
{
	this->useImgui = useImgui;
}

bool TinyEngine::isUseOpenXR() const
{
	return useOpenXR;
}

void TinyEngine::setUseOpenXR(bool useOpenXR)
{
	this->useOpenXR = useOpenXR;
}

