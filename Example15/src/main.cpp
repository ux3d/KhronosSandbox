#include "TinyEngine.h"

#include "XrEngine.h"

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
