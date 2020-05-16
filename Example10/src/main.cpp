#include "Application.h"

#include <GLFW/glfw3.h>

#define APP_WIDTH 1920
#define APP_HEIGHT 1080

int main(int argc, char **argv)
{
	std::string filename = "../Resources/glTF/Cube/Cube.gltf";

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

	Application application("Example10: glTF", filename, environment);
	application.setMinor(2);
	application.setDepthStencilFormat(VK_FORMAT_D24_UNORM_S8_UINT);
	application.setSamples(VK_SAMPLE_COUNT_4_BIT);
	application.addEnabledInstanceLayerName("VK_LAYER_KHRONOS_validation");
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensionNames = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	for (uint32_t i = 0; i < glfwExtensionCount; i++)
	{
		application.addEnabledInstanceExtensionName(glfwExtensionNames[i]);
	}

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

				application.orbitY(xpos - xposLast);
				application.orbitX(ypos - yposLast);

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

				application.zoom(ypos - yposLast);

				xposLast = xpos;
				yposLast = ypos;
			}
			else if (mouseRightPressed && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
			{
				mouseRightPressed = false;
			}

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
