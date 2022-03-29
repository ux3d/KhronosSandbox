#include "Application.h"
#include "XrEngine.h"

#define APP_TITLE "Example15: Hello OpenXR"

#include <GLFW/glfw3.h>

#define APP_WIDTH 1024
#define APP_HEIGHT 1024

int main(int argc, char **argv)
{
	GLFWwindow* window;

	if (!glfwInit())
	{
		return -1;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(APP_WIDTH, APP_HEIGHT, APP_TITLE, NULL, NULL);
	if (!window)
	{
		glfwTerminate();
	    return -1;
	}

	//

	XrEngine engine;

	if (!engine.prepare())
	{
	    return -1;
	}

	Application application;
	application.setApplicationName(APP_TITLE);
	application.setMajor(engine.getVulkanMajor());
	application.setMinor(engine.getVulkanMinor());
	application.addEnabledInstanceLayerName("VK_LAYER_KHRONOS_validation");
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensionNames = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	for (uint32_t i = 0; i < glfwExtensionCount; i++)
	{
		application.addEnabledInstanceExtensionName(glfwExtensionNames[i]);
	}
	for (uint32_t i = 0; i < engine.instanceExtensions.size(); i++)
	{
		application.addEnabledInstanceExtensionName(engine.instanceExtensions[i]);
	}

	if (!application.prepare())
	{
	    return -1;
	}

	VkSurfaceKHR surface = VK_NULL_HANDLE;
	VkResult result = glfwCreateWindowSurface(application.getInstance(), window, nullptr, &surface);
	if (result != VK_SUCCESS)
	{
		return -1;
	}

	if (!application.init(surface))
	{
		return -1;
	}

	//

	engine.m_vkInstance = application.getInstance();
	engine.m_vkPhysicalDevice = application.getPhysicalDevice();
	engine.m_vkDevice = application.getDevice();
	engine.m_vkQueueFamilyIndex = application.getQueueFamilyIndex();

	if (!engine.init())
	{
	    return -1;
	}

	while (!glfwWindowShouldClose(window))
	{
		if (!glfwGetWindowAttrib(window, GLFW_ICONIFIED))
		{
			if (!application.update())
			{
				break;
			}
		}

		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, 1);
		}
	}

	application.terminate();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
