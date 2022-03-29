#include "XrEngine.h"

#include <cstring>

#include "common/Common.h"

XrEngine::XrEngine()
{
}

XrEngine::~XrEngine()
{
}

bool XrEngine::bindFunctions()
{
	XrResult result = XR_SUCCESS;

    result = xrGetInstanceProcAddr(instance, "xrGetVulkanGraphicsRequirementsKHR", reinterpret_cast<PFN_xrVoidFunction*>(&pfn_xrGetVulkanGraphicsRequirementsKHR));
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

    result = xrGetInstanceProcAddr(instance, "xrGetVulkanGraphicsDeviceKHR", reinterpret_cast<PFN_xrVoidFunction*>(&pfn_xrGetVulkanGraphicsDeviceKHR));
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

    result = xrGetInstanceProcAddr(instance, "xrGetVulkanInstanceExtensionsKHR", reinterpret_cast<PFN_xrVoidFunction*>(&pfn_xrGetVulkanInstanceExtensionsKHR));
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	return true;
}

bool XrEngine::prepare()
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

	//

    XrGraphicsRequirementsVulkanKHR graphicsRequirementsVulkan{XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN_KHR};

	result = pfn_xrGetVulkanGraphicsRequirementsKHR(instance, systemId, &graphicsRequirementsVulkan);
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	vulkanMajor = (graphicsRequirementsVulkan.maxApiVersionSupported >> 48) & 0xFFFF;
	vulkanMinor = (graphicsRequirementsVulkan.maxApiVersionSupported >> 32) & 0xFFFF;
	vulkanPatch = (graphicsRequirementsVulkan.maxApiVersionSupported >>  0) & 0xFFFFFFFF;

	//

    uint32_t extensionNamesSize = 0;
    result = pfn_xrGetVulkanInstanceExtensionsKHR(instance, systemId, 0, &extensionNamesSize, nullptr);
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	instanceExtensionsString.resize(extensionNamesSize);
    result = pfn_xrGetVulkanInstanceExtensionsKHR(instance, systemId, extensionNamesSize, &extensionNamesSize, instanceExtensionsString.data());
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	char* names = &instanceExtensionsString[0];
	while (*names != 0)
	{
		instanceExtensions.push_back(names);
		while (*(++names) != 0)
		{
			if (*names == ' ')
			{
				*names++ = '\0';
				break;
			}
		}
	}

	return true;
}

uint32_t XrEngine::getVulkanMajor() const
{
	return vulkanMajor;
}

uint32_t XrEngine::getVulkanMinor() const
{
	return vulkanMinor;
}

uint32_t XrEngine::getVulkanPatch() const
{
	return vulkanPatch;
}

bool XrEngine::init()
{
	XrResult result = XR_SUCCESS;

	//

	VkPhysicalDevice vkPhysicalDevice;
	result = pfn_xrGetVulkanGraphicsDeviceKHR(instance, systemId, m_vkInstance, &vkPhysicalDevice);
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	//

	XrGraphicsBindingVulkanKHR graphicsBindingVulkan = {};
	graphicsBindingVulkan.type = XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR;
	graphicsBindingVulkan.instance = m_vkInstance;
	graphicsBindingVulkan.physicalDevice = m_vkPhysicalDevice;
	graphicsBindingVulkan.device = m_vkDevice;
	graphicsBindingVulkan.queueFamilyIndex = m_vkQueueFamilyIndex;
	graphicsBindingVulkan.queueIndex = 0;

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

	return true;
}

bool XrEngine::resize()
{
	return true;
}

bool XrEngine::update(uint32_t frameIndex, double deltaTime, double totalTime)
{
	XrResult result = XR_SUCCESS;

	//

	do {
		XrEventDataBuffer eventDataBuffer = {};
		eventDataBuffer.type = XR_TYPE_EVENT_DATA_BUFFER;

		result = xrPollEvent(instance, &eventDataBuffer);
		if (result == XR_SUCCESS)
		{
			switch (eventDataBuffer.type)
			{
				case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED:
				{
					const XrEventDataSessionStateChanged eventDataSessionStateChanged = *reinterpret_cast<const XrEventDataSessionStateChanged*>(&eventDataBuffer);

					sessionState = eventDataSessionStateChanged.state;
					switch (sessionState)
					{
						case XR_SESSION_STATE_IDLE:
						{
							Logger::print(TinyEngine_INFO, __FILE__, __LINE__, "OpenXR Session State Idle");

							break;
						}
						case XR_SESSION_STATE_READY:
						{
							Logger::print(TinyEngine_INFO, __FILE__, __LINE__, "OpenXR Session State Ready");

							XrSessionBeginInfo sessionBeginInfo = {};
							sessionBeginInfo.type = XR_TYPE_SESSION_BEGIN_INFO;
							sessionBeginInfo.primaryViewConfigurationType = viewConfigurationType;
							result = xrBeginSession(session, &sessionBeginInfo);
							if (result != XR_SUCCESS)
							{
								Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

								return false;
							}

							sessionRunning = true;

							break;
						}
						case XR_SESSION_STATE_SYNCHRONIZED:
						{
							Logger::print(TinyEngine_INFO, __FILE__, __LINE__, "OpenXR Session State Synchronized");

							break;
						}
						case XR_SESSION_STATE_VISIBLE:
						{
							Logger::print(TinyEngine_INFO, __FILE__, __LINE__, "OpenXR Session State Visible");

							break;
						}
						case XR_SESSION_STATE_FOCUSED:
						{
							Logger::print(TinyEngine_INFO, __FILE__, __LINE__, "OpenXR Session State Focused");

							break;
						}
						case XR_SESSION_STATE_STOPPING:
						{
							Logger::print(TinyEngine_INFO, __FILE__, __LINE__, "OpenXR Session State Stopping");

							result = xrEndSession(session);
							if (result != XR_SUCCESS)
							{
								Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

								return false;
							}

							sessionRunning = false;

							break;
						}
						case XR_SESSION_STATE_LOSS_PENDING:
						{
							Logger::print(TinyEngine_INFO, __FILE__, __LINE__, "OpenXR Session State Loss Pending");

							break;
						}
						case XR_SESSION_STATE_EXITING:
						{
							Logger::print(TinyEngine_INFO, __FILE__, __LINE__, "OpenXR Session State Exiting");

							break;
						}
						default:
							Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");
							return false;
					}

					break;
				}
				default:
					Logger::print(TinyEngine_INFO, __FILE__, __LINE__, "OpenXR Event Type: %u", eventDataBuffer.type);
			}
		}
		else if (result != XR_EVENT_UNAVAILABLE)
		{
			Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

			return false;
		}
	} while (result == XR_SUCCESS);

	//

	if (!sessionRunning)
	{
		return true;
	}

	//

	XrFrameWaitInfo frameWaitInfo = {};
	frameWaitInfo.type = XR_TYPE_FRAME_WAIT_INFO;

	XrFrameState frameState = {};
	frameState.type = XR_TYPE_FRAME_STATE;

	result = xrWaitFrame(session, &frameWaitInfo, &frameState);
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

    XrFrameBeginInfo frameBeginInfo = {};
    frameBeginInfo.type = XR_TYPE_FRAME_BEGIN_INFO;

    result = xrBeginFrame(session, &frameBeginInfo);
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

    std::vector<XrCompositionLayerBaseHeader*> layers;

	XrCompositionLayerProjection compositionLayerProjection = {};
	compositionLayerProjection.type = XR_TYPE_COMPOSITION_LAYER_PROJECTION;

    if (frameState.shouldRender)
    {
    	XrViewLocateInfo viewLocateInfo = {};
    	viewLocateInfo.type = XR_TYPE_VIEW_LOCATE_INFO;
        viewLocateInfo.viewConfigurationType = viewConfigurationType;
        viewLocateInfo.displayTime = frameState.predictedDisplayTime;
        viewLocateInfo.space = space;

        XrViewState viewState = {};
        viewState.type = XR_TYPE_VIEW_STATE;

        uint32_t viewCapacityInput = viewCount;
        uint32_t viewCountOutput = 0;

        result = xrLocateViews(session, &viewLocateInfo, &viewState, viewCapacityInput, &viewCountOutput, views.data());
    	if (result != XR_SUCCESS)
    	{
    		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

    		return false;
    	}

    	//

    	for (uint32_t i = 0; i < viewCount; i++)
    	{
            uint32_t imageArrayIndex = 0;
            XrSwapchainImageAcquireInfo swapchainImageAcquireInfo = {};
            swapchainImageAcquireInfo.type = XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO;

            result = xrAcquireSwapchainImage(swapchains[i], &swapchainImageAcquireInfo, &imageArrayIndex);
        	if (result != XR_SUCCESS)
        	{
        		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

        		return false;
        	}

            XrSwapchainImageWaitInfo swapchainImageWaitInfo = {};
            swapchainImageWaitInfo.type = XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO;
            swapchainImageWaitInfo.timeout = XR_INFINITE_DURATION;

            result = xrWaitSwapchainImage(swapchains[i], &swapchainImageWaitInfo);
        	if (result != XR_SUCCESS)
        	{
        		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

        		return false;
        	}

    		XrRect2Di imageRect = {};
            imageRect.offset.x = 0;
            imageRect.offset.y = 0;
            imageRect.extent.width = static_cast<int32_t>(viewConfigurationView[i].recommendedImageRectWidth);
            imageRect.extent.height = static_cast<int32_t>(viewConfigurationView[i].recommendedImageRectHeight);

    		compositionLayerProjectionView[i] = {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW};
    		compositionLayerProjectionView[i].pose = views[i].pose;
    		compositionLayerProjectionView[i].fov = views[i].fov;
    		compositionLayerProjectionView[i].subImage.swapchain = swapchains[i];
    		compositionLayerProjectionView[i].subImage.imageRect = imageRect;

    		//

        	// TODO: Add camera information.

        	//

            XrSwapchainImageReleaseInfo swapchainImageReleaseInfo = {};
            swapchainImageReleaseInfo.type = XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO;

            result = xrReleaseSwapchainImage(swapchains[i], &swapchainImageReleaseInfo);
        	if (result != XR_SUCCESS)
        	{
        		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

        		return false;
        	}
    	}

    	//

    	compositionLayerProjection.space = space;
    	compositionLayerProjection.viewCount = viewCount;
    	compositionLayerProjection.views = compositionLayerProjectionView.data();

    	layers.push_back(reinterpret_cast<XrCompositionLayerBaseHeader*>(&compositionLayerProjection));
    }

    XrFrameEndInfo frameEndInfo = {};
    frameEndInfo.type = XR_TYPE_FRAME_END_INFO;
    frameEndInfo.displayTime = frameState.predictedDisplayTime;
    frameEndInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
    frameEndInfo.layerCount = static_cast<uint32_t>(layers.size());
    frameEndInfo.layers = layers.data();

    result = xrEndFrame(session, &frameEndInfo);
	if (result != XR_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, "OpenXR");

		return false;
	}

	return true;
}

bool XrEngine::terminate()
{
	if (space != XR_NULL_HANDLE)
	{
		xrDestroySpace(space);
		space = XR_NULL_HANDLE;
	}

	compositionLayerProjectionView.clear();

	views.clear();

	swapchainImages.clear();

	for (uint32_t i = 0; i < viewCount; i++)
	{
		if (swapchains[i] != XR_NULL_HANDLE)
		{
			xrDestroySwapchain(swapchains[i]);
		}
	}
	swapchains.clear();

	viewConfigurationView.clear();

	viewCount = 0;

	if (session != XR_NULL_HANDLE)
	{
		xrDestroySession(session);
		session = XR_NULL_HANDLE;
	}
	sessionState = XR_SESSION_STATE_UNKNOWN;
	sessionRunning = false;

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
