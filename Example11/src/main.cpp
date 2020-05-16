#include "Application.h"

#include <GLFW/glfw3.h>

#define APP_WIDTH 1920
#define APP_HEIGHT 1080

int main()
{
	GLFWwindow* window;

	if (!glfwInit())
	{
		return -1;
	}

	Application application("Example11: Triangle raytraced");
	application.setMinor(2);
	application.addEnabledInstanceLayerName("VK_LAYER_KHRONOS_validation");
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensionNames = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	for (uint32_t i = 0; i < glfwExtensionCount; i++)
	{
		application.addEnabledInstanceExtensionName(glfwExtensionNames[i]);
	}
	application.addEnabledDeviceExtensionName(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
	application.addEnabledDeviceExtensionName(VK_KHR_RAY_TRACING_EXTENSION_NAME);
	application.addEnabledDeviceExtensionName(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);	// Required by ray tracing.
	application.addEnabledDeviceExtensionName(VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME);			// Required by ray tracing.
	application.setImageUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(APP_WIDTH, APP_HEIGHT, application.getTitle(), NULL, NULL);
	if (!window)
	{
		glfwTerminate();
	    return -1;
	}

	if (!application.prepare())
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	    return -1;
	}

	VkSurfaceKHR surface = VK_NULL_HANDLE;
	VkResult result = glfwCreateWindowSurface(application.getInstance(), window, nullptr, &surface);
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEnigne_ERROR, __FILE__, __LINE__, result);
		application.terminate();
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	if (!application.init(surface, APP_WIDTH, APP_HEIGHT))
	{
		application.terminate();
		glfwDestroyWindow(window);
		glfwTerminate();
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
