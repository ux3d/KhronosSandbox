#ifndef XRENGINE_H_
#define XRENGINE_H_

#include "BaseEngine.h"

#include <cstdint>
#include <vector>

#include "composite/Composite.h"

#define XR_USE_GRAPHICS_API_VULKAN
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

class XrEngine : public BaseEngine
{

private:

	PFN_xrGetVulkanGraphicsRequirementsKHR pfn_xrGetVulkanGraphicsRequirementsKHR = nullptr;
	PFN_xrGetVulkanGraphicsDeviceKHR pfn_xrGetVulkanGraphicsDeviceKHR = nullptr;
	PFN_xrGetVulkanInstanceExtensionsKHR pfn_xrGetVulkanInstanceExtensionsKHR = nullptr;
	PFN_xrGetVulkanDeviceExtensionsKHR pfn_xrGetVulkanDeviceExtensionsKHR = nullptr;

	XrInstance xrInstance = XR_NULL_HANDLE;
	XrSystemId xrSystemId = XR_NULL_SYSTEM_ID;
	XrViewConfigurationType xrViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
	XrEnvironmentBlendMode xrEnvironmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;

	std::vector<char> xrInstanceExtensionsString;

	XrSession xrSession = XR_NULL_HANDLE;

	bool bindFunctions();

public:
	XrEngine();
	virtual ~XrEngine();

	bool prepare();

	bool init();
	bool resize();
	bool update(uint32_t frameIndex, double deltaTime, double totalTime);
	bool terminate();

};

#endif /* XRENGINE_H_ */
