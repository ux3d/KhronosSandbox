#include "XrEngine.h"

#include <cstring>

XrEngine::XrEngine()
{
}

XrEngine::~XrEngine()
{
}

bool XrEngine::bindFunctions()
{
	XrResult result = XR_SUCCESS;

    result = xrGetInstanceProcAddr(instance, "xrGetVulkanInstanceExtensionsKHR", reinterpret_cast<PFN_xrVoidFunction*>(&pfnGetVulkanInstanceExtensionsKHR));
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

    result = xrGetInstanceProcAddr(instance, "xrGetVulkanDeviceExtensionsKHR", reinterpret_cast<PFN_xrVoidFunction*>(&pfnGetVulkanDeviceExtensionsKHR));
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

    result = xrGetInstanceProcAddr(instance, "xrGetVulkanGraphicsDeviceKHR", reinterpret_cast<PFN_xrVoidFunction*>(&pfnGetVulkanGraphicsDeviceKHR));
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

    result = xrGetInstanceProcAddr(instance, "xrGetVulkanGraphicsRequirementsKHR", reinterpret_cast<PFN_xrVoidFunction*>(&pfnGetVulkanGraphicsRequirementsKHR));
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	return true;
}

bool XrEngine::prepare()
{
	return true;
}

bool XrEngine::init(VkInstance vulkanInstance, VkPhysicalDevice vulkanPhysicalDevice, VkDevice vulkanDevice, uint32_t vulkanQueueFamilyIndex, uint32_t vulkanQueueIndex, VkFormat vulkanFormat)
{
	std::vector<const char*> enabledInstanceExtensionNames;
	enabledInstanceExtensionNames.push_back(XR_KHR_VULKAN_ENABLE_EXTENSION_NAME);

	//

	XrResult result = XR_SUCCESS;

    XrInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.type = XR_TYPE_INSTANCE_CREATE_INFO;

    instanceCreateInfo.applicationInfo = {};
    strncpy(instanceCreateInfo.applicationInfo.applicationName, "Hello OpenXR", XR_MAX_APPLICATION_NAME_SIZE);
    instanceCreateInfo.applicationInfo.applicationVersion = 1;
    strncpy(instanceCreateInfo.applicationInfo.engineName, "XrEngine", XR_MAX_ENGINE_NAME_SIZE);
    instanceCreateInfo.applicationInfo.engineVersion = 1;
    instanceCreateInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledInstanceExtensionNames.size());
	instanceCreateInfo.enabledExtensionNames = enabledInstanceExtensionNames.data();

    result = xrCreateInstance(&instanceCreateInfo, &instance);
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	//

	if (!bindFunctions())
	{
		return false;
	}

	//

    XrSystemGetInfo systemGetInfo = {};
    systemGetInfo.type = XR_TYPE_SYSTEM_GET_INFO;
    systemGetInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

    result = xrGetSystem(instance, &systemGetInfo, &systemId);
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

    XrSystemProperties systemProperties = {};
    systemProperties.type = XR_TYPE_SYSTEM_PROPERTIES;

    result = xrGetSystemProperties(instance, systemId, &systemProperties);
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	Logger::print(TinyEngine_INFO, __FILE__, __LINE__, "System Name: '%s'", systemProperties.systemName);

	//

	uint32_t environmentBlendModeCount = 0;
	result = xrEnumerateEnvironmentBlendModes(instance, systemId, viewConfigurationType, 0, &environmentBlendModeCount, nullptr);
	if (result != XR_SUCCESS || environmentBlendModeCount == 0)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	std::vector<XrEnvironmentBlendMode> environmentBlendModes(environmentBlendModeCount);
	result = xrEnumerateEnvironmentBlendModes(instance, systemId, viewConfigurationType, environmentBlendModeCount, &environmentBlendModeCount, environmentBlendModes.data());
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	environmentBlendMode = environmentBlendModes[0];

	// Vulkan specific

	XrGraphicsRequirementsVulkanKHR graphicsRequirementsVulkan = {};
	graphicsRequirementsVulkan.type = XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN_KHR;

	result = pfnGetVulkanGraphicsRequirementsKHR(instance, systemId, &graphicsRequirementsVulkan);
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	XrGraphicsBindingVulkanKHR graphicsBindingVulkan = {};
	graphicsBindingVulkan.type = XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR;
	graphicsBindingVulkan.instance = vulkanInstance;
	graphicsBindingVulkan.physicalDevice = vulkanPhysicalDevice;
	graphicsBindingVulkan.device = vulkanDevice;
	graphicsBindingVulkan.queueFamilyIndex = vulkanQueueFamilyIndex;
	graphicsBindingVulkan.queueIndex = vulkanQueueIndex;

	//

    XrSessionCreateInfo sessionCreateInfo = {};
    sessionCreateInfo.type = XR_TYPE_SESSION_CREATE_INFO;
    sessionCreateInfo.next = &graphicsBindingVulkan;
    sessionCreateInfo.systemId = systemId;
    result = xrCreateSession(instance, &sessionCreateInfo, &session);
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	//

	uint32_t formatCount = 0;
	result = xrEnumerateSwapchainFormats(session, 0, &formatCount, nullptr);
	if (result != XR_SUCCESS || formatCount == 0)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

    std::vector<int64_t> formats(formatCount);
    result = xrEnumerateSwapchainFormats(session, formatCount, &formatCount, formats.data());
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	int64_t swapchainFormat = 0;
	for (int64_t currentFormat : formats)
	{
		if ((VkFormat)currentFormat == vulkanFormat)
		{
			swapchainFormat = currentFormat;
			break;
		}
	}

	if (swapchainFormat == 0)
	{
		return false;
	}

	//

    uint32_t viewCount = 0;
    result = xrEnumerateViewConfigurationViews(instance, systemId, viewConfigurationType, 0, &viewCount, nullptr);
	if (result != XR_SUCCESS || viewCount == 0)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	std::vector<XrViewConfigurationView> viewConfigurationView(viewCount);
	result = xrEnumerateViewConfigurationViews(instance, systemId, viewConfigurationType, viewCount, &viewCount, viewConfigurationView.data());
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	swapchains.resize(viewCount);

	for (uint32_t i = 0; i < viewCount; i++)
	{
	    XrSwapchainCreateInfo swapchainCreateInfo = {};
	    swapchainCreateInfo.type = XR_TYPE_SWAPCHAIN_CREATE_INFO;
	    swapchainCreateInfo.arraySize = 1;
	    swapchainCreateInfo.format = swapchainFormat;
	    swapchainCreateInfo.width = viewConfigurationView[i].recommendedImageRectWidth;
	    swapchainCreateInfo.height = viewConfigurationView[i].recommendedImageRectHeight;
	    swapchainCreateInfo.mipCount = 1;
	    swapchainCreateInfo.faceCount = 1;
	    swapchainCreateInfo.sampleCount = viewConfigurationView[i].recommendedSwapchainSampleCount;
	    swapchainCreateInfo.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;

	    result = xrCreateSwapchain(session, &swapchainCreateInfo, &swapchains[i]);
		if (result != XR_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

			return false;
		}
	}

	return true;
}

bool XrEngine::resize()
{
	return true;
}

bool XrEngine::update()
{
	return true;
}

bool XrEngine::terminate()
{
	for (XrSwapchain swapchain : swapchains)
	{
		xrDestroySwapchain(swapchain);
	}
	swapchains.clear();

	if (session != XR_NULL_HANDLE)
	{
		xrDestroySession(session);
		session = XR_NULL_HANDLE;
	}

	environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;

	viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;

	systemId = XR_NULL_SYSTEM_ID;

	if (instance != XR_NULL_HANDLE)
	{
		xrDestroyInstance(instance);
		instance = XR_NULL_HANDLE;
	}

	return true;
}
