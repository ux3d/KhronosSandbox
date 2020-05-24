#include "Application.h"

#include <GLFW/glfw3.h>

#define APP_WIDTH 1920
#define APP_HEIGHT 1080

int main(int argc, char **argv)
{
	std::string filename = "../Resources/glTF/Sphere/Sphere.gltf";

	std::string environment = "../Resources/brdf/doge2";

	if (argc > 1)
	{
		filename = argv[1];

		if (argc > 2)
		{
			environment = argv[2];
		}
	}

	GLFWwindow* window;

	if (!glfwInit())
	{
		return -1;
	}

	Application application("glTF 2.0 raytrace vs. pathtrace vs. rasterize using Vulkan", filename, environment);
	application.setUseImgui(true);
	application.setMinor(2);
	application.setDepthStencilFormat(VK_FORMAT_D24_UNORM_S8_UINT);
	application.addEnabledInstanceLayerName("VK_LAYER_KHRONOS_validation");
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensionNames = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	for (uint32_t i = 0; i < glfwExtensionCount; i++)
	{
		application.addEnabledInstanceExtensionName(glfwExtensionNames[i]);
	}
	application.addEnabledDeviceExtensionName(VK_EXT_INDEX_TYPE_UINT8_EXTENSION_NAME);			// Required by for glTF.
	application.addEnabledDeviceExtensionName(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
	application.addEnabledDeviceExtensionName(VK_KHR_RAY_TRACING_EXTENSION_NAME);
	application.addEnabledDeviceExtensionName(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);	// Required by ray tracing.
	application.addEnabledDeviceExtensionName(VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME);			// Required by ray tracing.
	application.addEnabledDeviceExtensionName(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);	    // Required by sophisticated ray tracing.
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

	ImGui::CreateContext();
	if (!ImGui_ImplGlfw_InitForVulkan(window, true))
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

	bool mouseLeftPressed = false;
	bool mouseRightPressed = false;
	double xposLast = 0.0;
	double yposLast = 0.0;

	while (!glfwWindowShouldClose(window))
	{
		if (!glfwGetWindowAttrib(window, GLFW_ICONIFIED))
		{
			// Orbit
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			{
				double xpos;
				double ypos;

				glfwGetCursorPos(window, &xpos, &ypos);
				if (!mouseLeftPressed)
				{
					mouseLeftPressed = true;

					xposLast = xpos;
					yposLast = ypos;
				}

				application.orbitY(static_cast<float>(xpos - xposLast));
				application.orbitX(static_cast<float>(ypos - yposLast));

				xposLast = xpos;
				yposLast = ypos;
			}
			else if (mouseLeftPressed && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
			{
				mouseLeftPressed = false;
			}

			// Zoom
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
			{
				double xpos;
				double ypos;

				glfwGetCursorPos(window, &xpos, &ypos);
				if (!mouseRightPressed)
				{
					mouseRightPressed = true;

					xposLast = xpos;
					yposLast = ypos;
				}

				application.zoom(static_cast<float>(ypos - yposLast));

				xposLast = xpos;
				yposLast = ypos;
			}
			else if (mouseRightPressed && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
			{
				mouseRightPressed = false;
			}

			ImGui_ImplGlfw_NewFrame();

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

	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
