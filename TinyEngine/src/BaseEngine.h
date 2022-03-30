#ifndef BASEENGINE_H_
#define BASEENGINE_H_

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "common/Common.h"

class BaseEngine {
protected:

	std::string applicationName = "TinyEngineApp";
	uint32_t major = 1;
	uint32_t minor = 0;
	uint32_t patch = 0;

	std::vector<const char*> enabledInstanceLayerNames = {};
	std::vector<const char*> enabledInstanceExtensionNames = {};

	std::vector<const char*> enabledDeviceExtensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	VkInstance instance = VK_NULL_HANDLE;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	VkSurfaceKHR surface = VK_NULL_HANDLE;

	std::optional<uint32_t> queueFamilyIndex;
	VkQueue queue = VK_NULL_HANDLE;

	VkDevice device = VK_NULL_HANDLE;

	bool inResize = false;

	bool createInstance();

	bool createDevice();

public:
	BaseEngine();
	virtual ~BaseEngine();

	bool hasEnabledInstanceExtensionName(const char* extensionName) const;
	bool hasEnabledDeviceExtensionName(const char* extensionName) const;

};

#endif /* BASEENGINE_H_ */
