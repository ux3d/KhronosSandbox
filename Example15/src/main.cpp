#include "Application.h"
#include "XrEngine.h"

#define APP_TITLE "Example15: Hello OpenXR"

int main(int argc, char **argv)
{
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

	application.addEnabledInstanceExtensionName("VK_KHR_surface");
	// TODO: Make platform generic
	application.addEnabledInstanceExtensionName("VK_KHR_win32_surface");

	if (!application.prepare())
	{
	    return -1;
	}

	if (!engine.init())
	{
	    return -1;
	}

	return 0;
}
