#include "BaseEngine.h"

#include <cstring>

// Protected

bool BaseEngine::createInstance()
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

bool BaseEngine::createDevice()
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
		if (surface != VK_NULL_HANDLE)
		{
			result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, currentQueueFamilyIndex, surface, &surfaceSupport);
			if (result != VK_SUCCESS)
			{
				Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);

				return false;
			}
		}

		if ((queueFamilyProperties[currentQueueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) && (surfaceSupport || surface == VK_NULL_HANDLE))
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

BaseEngine::BaseEngine()
{
}

BaseEngine::~BaseEngine()
{
}

bool BaseEngine::hasEnabledInstanceExtensionName(const char* extensionName) const
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


bool BaseEngine::hasEnabledDeviceExtensionName(const char* extensionName) const
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
