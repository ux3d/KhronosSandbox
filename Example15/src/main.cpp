#include "XrEngine.h"

#define APP_TITLE "Example15: Hello OpenXR"

int main(int argc, char **argv)
{
	XrEngine engine;

	if (!engine.prepare())
	{
	    return -1;
	}

	if (!engine.init())
	{
	    return -1;
	}

	return 0;
}
