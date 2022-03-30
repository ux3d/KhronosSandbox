#include "XrEngine.h"

#include "TinyEngine.h"

// Private

bool XrEngine::bindFunctions()
{
	XrResult result = XR_SUCCESS;

    result = xrGetInstanceProcAddr(xrInstance, "xrGetVulkanGraphicsRequirementsKHR", reinterpret_cast<PFN_xrVoidFunction*>(&pfn_xrGetVulkanGraphicsRequirementsKHR));
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

    result = xrGetInstanceProcAddr(xrInstance, "xrGetVulkanGraphicsDeviceKHR", reinterpret_cast<PFN_xrVoidFunction*>(&pfn_xrGetVulkanGraphicsDeviceKHR));
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

    result = xrGetInstanceProcAddr(xrInstance, "xrGetVulkanInstanceExtensionsKHR", reinterpret_cast<PFN_xrVoidFunction*>(&pfn_xrGetVulkanInstanceExtensionsKHR));
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

    result = xrGetInstanceProcAddr(xrInstance, "xrGetVulkanDeviceExtensionsKHR", reinterpret_cast<PFN_xrVoidFunction*>(&pfn_xrGetVulkanDeviceExtensionsKHR));
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	return true;
}

XrEngine::XrEngine() : BaseEngine()
{
}

XrEngine::~XrEngine()
{
}

bool XrEngine::prepare()
{
	std::vector<const char*> enabledApiLayerNames;
	enabledApiLayerNames.push_back("XR_APILAYER_LUNARG_core_validation");
	enabledApiLayerNames.push_back("XR_APILAYER_LUNARG_api_dump");

	std::vector<const char*> enabledExtensionNames;
	enabledExtensionNames.push_back(XR_KHR_VULKAN_ENABLE_EXTENSION_NAME);

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


    instanceCreateInfo.enabledApiLayerCount = static_cast<uint32_t>(enabledApiLayerNames.size());
    instanceCreateInfo.enabledApiLayerNames = enabledApiLayerNames.data();

    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensionNames.size());
	instanceCreateInfo.enabledExtensionNames = enabledExtensionNames.data();

    result = xrCreateInstance(&instanceCreateInfo, &xrInstance);
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

	XrInstanceProperties instanceProperties{XR_TYPE_INSTANCE_PROPERTIES};
	result = xrGetInstanceProperties(xrInstance, &instanceProperties);
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	//

    XrSystemGetInfo systemGetInfo = {};
    systemGetInfo.type = XR_TYPE_SYSTEM_GET_INFO;
    systemGetInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

    result = xrGetSystem(xrInstance, &systemGetInfo, &xrSystemId);
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	//

	uint32_t viewConfigurationTypeCountOutput;
	result = xrEnumerateViewConfigurations(xrInstance, xrSystemId, 0, &viewConfigurationTypeCountOutput, nullptr);
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

    std::vector<XrViewConfigurationType> viewConfigurationTypes(viewConfigurationTypeCountOutput);
	result = xrEnumerateViewConfigurations(xrInstance, xrSystemId, viewConfigurationTypeCountOutput, &viewConfigurationTypeCountOutput, viewConfigurationTypes.data());
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	//

	for (XrViewConfigurationType& viewConfigurationType : viewConfigurationTypes)
	{
		XrViewConfigurationProperties viewConfigurationProperties{XR_TYPE_VIEW_CONFIGURATION_PROPERTIES};
		result = xrGetViewConfigurationProperties(xrInstance, xrSystemId, viewConfigurationType, &viewConfigurationProperties);
		if (result != XR_SUCCESS)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

			return false;
		}

		uint32_t viewCount;
		result = xrEnumerateViewConfigurationViews(xrInstance, xrSystemId, viewConfigurationType, 0, &viewCount, nullptr);
		if (result != XR_SUCCESS || viewCount == 0)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

			return false;
		}

		std::vector<XrViewConfigurationView> viewConfigurationViews(viewCount, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
		result = xrEnumerateViewConfigurationViews(xrInstance, xrSystemId, viewConfigurationType, viewCount, &viewCount, viewConfigurationViews.data());

		for (uint32_t i = 0; i < viewConfigurationViews.size(); i++)
		{
			const XrViewConfigurationView& currentViewConfigurationView = viewConfigurationViews[i];

			Logger::print(TinyEngine_INFO, __FILE__, __LINE__, "XrViewConfigurationView #%u %u %u %u", i, currentViewConfigurationView.recommendedImageRectWidth, currentViewConfigurationView.recommendedImageRectHeight, currentViewConfigurationView.recommendedSwapchainSampleCount);
		}
	}

	uint32_t environmentBlendModeCount = 0;
	result = xrEnumerateEnvironmentBlendModes(xrInstance, xrSystemId, xrViewConfigurationType, 0, &environmentBlendModeCount, nullptr);
	if (result != XR_SUCCESS || environmentBlendModeCount == 0)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	std::vector<XrEnvironmentBlendMode> environmentBlendModes(environmentBlendModeCount);
	result = xrEnumerateEnvironmentBlendModes(xrInstance, xrSystemId, xrViewConfigurationType, environmentBlendModeCount, &environmentBlendModeCount, environmentBlendModes.data());
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	xrEnvironmentBlendMode = environmentBlendModes[0];

	//
	// Vulkan dependent
	//

    XrGraphicsRequirementsVulkanKHR graphicsRequirementsVulkan{XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN_KHR};

	result = pfn_xrGetVulkanGraphicsRequirementsKHR(xrInstance, xrSystemId, &graphicsRequirementsVulkan);
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	major = (graphicsRequirementsVulkan.maxApiVersionSupported >> 48) & 0xFFFF;
	minor = (graphicsRequirementsVulkan.maxApiVersionSupported >> 32) & 0xFFFF;
	patch = (graphicsRequirementsVulkan.maxApiVersionSupported >>  0) & 0xFFFFFFFF;

	//

    uint32_t extensionNamesSize = 0;
    result = pfn_xrGetVulkanInstanceExtensionsKHR(xrInstance, xrSystemId, 0, &extensionNamesSize, nullptr);
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	xrInstanceExtensionsString.resize(extensionNamesSize);
    result = pfn_xrGetVulkanInstanceExtensionsKHR(xrInstance, xrSystemId, extensionNamesSize, &extensionNamesSize, xrInstanceExtensionsString.data());
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	char* names = &xrInstanceExtensionsString[0];
	while (*names != 0)
	{
		enabledInstanceExtensionNames.push_back(names);
		while (*(++names) != 0)
		{
			if (*names == ' ')
			{
				*names++ = '\0';
				break;
			}
		}
	}

	//
	// Non-Vulkan dependent
	//

    XrSystemProperties systemProperties = {};
    systemProperties.type = XR_TYPE_SYSTEM_PROPERTIES;

    result = xrGetSystemProperties(xrInstance, xrSystemId, &systemProperties);
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	Logger::print(TinyEngine_INFO, __FILE__, __LINE__, "System Name: '%s'", systemProperties.systemName);

	return true;
}

bool XrEngine::init()
{
	XrResult result = XR_SUCCESS;

	//

	if (!createInstance())
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	//

	result = pfn_xrGetVulkanGraphicsDeviceKHR(xrInstance, xrSystemId, instance, &physicalDevice);
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	// TODO: From here, values maybe are not initialized and/or correct

	XrGraphicsBindingVulkanKHR graphicsBindingVulkan = {};
	graphicsBindingVulkan.type = XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR;
	graphicsBindingVulkan.instance = instance;
	graphicsBindingVulkan.physicalDevice = physicalDevice;
	graphicsBindingVulkan.device = VK_NULL_HANDLE;			// TODO
	graphicsBindingVulkan.queueFamilyIndex = 0;				// TODO
	graphicsBindingVulkan.queueIndex = 0;

	//

    XrSessionCreateInfo sessionCreateInfo = {};
    sessionCreateInfo.type = XR_TYPE_SESSION_CREATE_INFO;
    sessionCreateInfo.next = &graphicsBindingVulkan;
    sessionCreateInfo.systemId = xrSystemId;
    result = xrCreateSession(xrInstance, &sessionCreateInfo, &xrSession);
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	return true;
}

bool XrEngine::resize()
{
	return true;
}

bool XrEngine::update(uint32_t frameIndex, double deltaTime, double totalTime)
{
	return true;
}

bool XrEngine::terminate()
{
	// TODO: Session termination

	if (!inResize)
	{
		physicalDevice = VK_NULL_HANDLE;

		if (instance)
		{
			vkDestroyInstance(instance, nullptr);
			instance = VK_NULL_HANDLE;
		}
	}

	if (xrInstance != XR_NULL_HANDLE)
	{
		xrDestroyInstance(xrInstance);
		xrInstance = XR_NULL_HANDLE;
	}

	return true;
}
