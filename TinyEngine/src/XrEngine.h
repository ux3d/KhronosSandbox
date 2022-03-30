#ifndef XRENGINE_H_
#define XRENGINE_H_

#include <cstdint>
#include <vector>

#include "composite/Composite.h"

#define XR_USE_GRAPHICS_API_VULKAN
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

class XrEngine
{

private:

	PFN_xrGetVulkanGraphicsRequirementsKHR pfn_xrGetVulkanGraphicsRequirementsKHR = nullptr;
	PFN_xrGetVulkanGraphicsDeviceKHR pfn_xrGetVulkanGraphicsDeviceKHR = nullptr;
	PFN_xrGetVulkanInstanceExtensionsKHR pfn_xrGetVulkanInstanceExtensionsKHR = nullptr;
	PFN_xrGetVulkanDeviceExtensionsKHR pfn_xrGetVulkanDeviceExtensionsKHR = nullptr;

	XrInstance instance = XR_NULL_HANDLE;
	XrSystemId systemId = XR_NULL_SYSTEM_ID;
	XrViewConfigurationType viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
	XrEnvironmentBlendMode environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;

	uint32_t vulkanMajor = 1;
	uint32_t vulkanMinor = 0;
	uint32_t vulkanPatch = 0;

	std::vector<const char*> vulkanEnabledInstanceLayerNames = { "VK_LAYER_KHRONOS_validation" };
	std::vector<const char*> vulkanEnabledInstanceExtensionNames = {};

	std::vector<const char*> vulkanEnabledDeviceExtensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };


	std::vector<char> instanceExtensionsString;

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

    VkInstance m_vkInstance{VK_NULL_HANDLE};
    VkPhysicalDevice m_vkPhysicalDevice{VK_NULL_HANDLE};
    VkDevice m_vkDevice{VK_NULL_HANDLE};
    uint32_t m_vkQueueFamilyIndex = 0;
    VkQueue m_vkQueue{VK_NULL_HANDLE};
    VkSemaphore m_vkDrawDone{VK_NULL_HANDLE};

    bool createInstance();

	bool bindFunctions();

public:
	XrEngine();
	virtual ~XrEngine();

	bool prepare();

	uint32_t getVulkanMajor() const;
	uint32_t getVulkanMinor() const;
	uint32_t getVulkanPatch() const;

	bool init();
	bool resize();
	bool update(uint32_t frameIndex, double deltaTime, double totalTime);
	bool terminate();

};

#endif /* XRENGINE_H_ */
