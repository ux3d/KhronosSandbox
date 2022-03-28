#include "Application.h"

#define APP_TITLE "Example15: Hello OpenXR"

int main(int argc, char **argv)
{
	XrEngine engine;

	Application application;
	application.setApplicationName(APP_TITLE);
	application.addEnabledInstanceLayerName("VK_LAYER_KHRONOS_validation");

	if (!application.prepare())
	{
	    return -1;
	}

	application.terminate();

	return 0;
}
