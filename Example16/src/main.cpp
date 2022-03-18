#include "Application.h"

#include <GLFW/glfw3.h>

#define APP_WIDTH 1920
#define APP_HEIGHT 1080
#define APP_TITLE "Example16: HDR tests"

int main()
{
	GLFWwindow* window;

	if (!glfwInit())
	{
		return -1;
	}

	Application application;
	application.setApplicationName(APP_TITLE);
	application.setMinor(2);
	application.addEnabledInstanceLayerName("VK_LAYER_KHRONOS_validation");

	//

	// Note: Execute vulkaninfo to gather allowed combinations for your display and graphics card.
	//
	// Display: 		Samsung UE75NU8009 UHD 4K
	// Graphics card:	NVIDIA GeForce RTX 2060
	//
	// First version is available in most cases.
	//

	application.setColorFormat(VK_FORMAT_B8G8R8A8_UNORM);
	application.setColorSpace(VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);

	/*
	application.setColorFormat(VK_FORMAT_B8G8R8A8_SRGB);
	application.setColorSpace(VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
	*/

	/*
	application.setColorFormat(VK_FORMAT_A2B10G10R10_UNORM_PACK32);
	application.setColorSpace(VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
	*/

	/*
	application.setColorFormat(VK_FORMAT_R16G16B16A16_SFLOAT);
	application.setColorSpace(VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT);
	*/

	/*
	application.setColorFormat(VK_FORMAT_A2B10G10R10_UNORM_PACK32);
	application.setColorSpace(VK_COLOR_SPACE_HDR10_ST2084_EXT);
	*/

	//

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensionNames = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	for (uint32_t i = 0; i < glfwExtensionCount; i++)
	{
		application.addEnabledInstanceExtensionName(glfwExtensionNames[i]);
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	window = glfwCreateWindow(APP_WIDTH, APP_HEIGHT, APP_TITLE, NULL, NULL);
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
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);
		application.terminate();
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	if (!application.init(surface))
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
