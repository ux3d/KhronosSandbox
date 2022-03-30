#include "BaseEngine.h"

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

BaseEngine::BaseEngine()
{
}

BaseEngine::~BaseEngine()
{
}
