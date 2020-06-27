#ifndef XRENGINE_H_
#define XRENGINE_H_

#include "TinyEngine.h"

#include <openxr/openxr.h>

class XrEngine
{

private:

	XrInstance instance = XR_NULL_HANDLE;
	XrSystemId systemId = XR_NULL_SYSTEM_ID;

public:
	XrEngine();
	virtual ~XrEngine();

	bool prepare();

	bool init();
	bool resize();
	bool update();
	bool terminate();

};

#endif /* XRENGINE_H_ */
