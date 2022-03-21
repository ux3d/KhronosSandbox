#include "Application.h"

#include <GLFW/glfw3.h>

#include "query_display.h"

#define APP_WIDTH 1920
#define APP_HEIGHT 1080
#define APP_TITLE "Example16: HDR tests"

int main(int argc, char *argv[])
{
	VkHdrMetadataEXT hdrMetadata;
	if (!getHdrMetdata(hdrMetadata))
	{
		return -1;
	}

	//

	// 0 None
	// 1 Reinhard
	// 2 Extended Reinhard
	// 3 Reinhard Jodie
	// 4 ACES (Hill)
	// 5 ACES (Narkowicz)
	// 6 Extended Reinhard Luminance
	// 7 Uncharted 2
	int32_t tonemap = 4;

	// 0 HDR image
	// 1 LDR image
	int32_t testImage = 0;

	// see below
	int32_t surfaceFormat = 0;

	// Maximum scene white
	float maxWhite = 1.0f;

	// Artistic changes
	float outputFactor = 1.0f;

	// If true, all pixels having a channel larger than 1.0 do become red.
	bool debug = false;

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-t") == 0 && (i + 1 < argc))
        {
            tonemap = (int32_t)std::stoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-i") == 0 && (i + 1 < argc))
        {
            testImage = (int32_t)std::stoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-s") == 0 && (i + 1 < argc))
        {
        	surfaceFormat = (int32_t)std::stoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-w") == 0 && (i + 1 < argc))
        {
        	maxWhite = std::stof(argv[i + 1]);
        }
        if (strcmp(argv[i], "-o") == 0 && (i + 1 < argc))
        {
        	outputFactor = std::stof(argv[i + 1]);
        }
		if (strcmp(argv[i], "-d") == 0 && (i + 1 < argc))
		{
			if (strcmp(argv[i + 1], "true") == 0)
			{
				debug = "true";
			}
		}
    }

	hdrMetadata.maxContentLightLevel *= outputFactor;

	//
	//
	//

	GLFWwindow* window;

	if (!glfwInit())
	{
		return -1;
	}

	Application application(tonemap, testImage, hdrMetadata, maxWhite, debug);
	application.setApplicationName(APP_TITLE);
	application.setMinor(2);
	application.addEnabledInstanceLayerName("VK_LAYER_KHRONOS_validation");

	//
	// Execute vulkaninfo to gather allowed combinations for your display and graphics card.
	// Following surface list is from the given setup:
	// Display: 		Samsung UE75NU8009 UHD 4K
	// Graphics card:	NVIDIA GeForce RTX 2060
	//
	// SurfaceFormat 0 is available in most cases. This index is not the one from the vulkaninfo list.
	//

	switch (surfaceFormat)
	{
		case 0:
			application.setColorFormat(VK_FORMAT_B8G8R8A8_UNORM);
			application.setColorSpace(VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
		break;
		case 1:
			application.setColorFormat(VK_FORMAT_B8G8R8A8_SRGB);
			application.setColorSpace(VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
		break;
		case 2:
			application.setColorFormat(VK_FORMAT_A2B10G10R10_UNORM_PACK32);
			application.setColorSpace(VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
		break;
		case 3:
			application.setColorFormat(VK_FORMAT_R16G16B16A16_SFLOAT);
			application.setColorSpace(VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT);
		break;
		case 4:
			application.setColorFormat(VK_FORMAT_A2B10G10R10_UNORM_PACK32);
			application.setColorSpace(VK_COLOR_SPACE_HDR10_ST2084_EXT);
		break;
		default:
			glfwTerminate();
		    return -1;
	}

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
