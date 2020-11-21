#ifndef XRENGINE_H_
#define XRENGINE_H_

#include <cstdint>
#include <vector>

#include "composite/Composite.h"

#include "IXr.h"

#define XR_USE_GRAPHICS_API_VULKAN
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

class XrEngine
{

private:

	PFN_xrGetVulkanInstanceExtensionsKHR pfnGetVulkanInstanceExtensionsKHR = nullptr;
	PFN_xrGetVulkanDeviceExtensionsKHR pfnGetVulkanDeviceExtensionsKHR = nullptr;
	PFN_xrGetVulkanGraphicsDeviceKHR pfnGetVulkanGraphicsDeviceKHR = nullptr;
	PFN_xrGetVulkanGraphicsRequirementsKHR pfnGetVulkanGraphicsRequirementsKHR = nullptr;

	XrInstance instance = XR_NULL_HANDLE;
	XrSystemId systemId = XR_NULL_SYSTEM_ID;
	XrViewConfigurationType viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
	XrEnvironmentBlendMode environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
	XrSession session = XR_NULL_HANDLE;
	XrSessionState sessionState = XR_SESSION_STATE_UNKNOWN;
	bool sessionRunning = false;

    uint32_t viewCount = 0;
    std::vector<XrViewConfigurationView> viewConfigurationView;
    std::vector<XrSwapchain> swapchains;
    std::vector<XrSwapchainImageVulkanKHR> swapchainImages;
	std::vector<XrView> views;
	std::vector<XrCompositionLayerProjectionView> compositionLayerProjectionView;

	XrSpace space = XR_NULL_HANDLE;

	bool bindFunctions();

public:
	XrEngine();
	virtual ~XrEngine();

	bool prepare();

	bool init(VkInstance vulkanInstance, VkPhysicalDevice vulkanPhysicalDevice, VkDevice vulkanDevice, uint32_t vulkanQueueFamilyIndex, uint32_t vulkanQueueIndex, VkFormat vulkanFormat, IXr& xr);
	bool resize();
	bool update(uint32_t frameIndex, double deltaTime, double totalTime, IXr& xr);
	bool terminate(IXr& xr);

};

#endif /* XRENGINE_H_ */
