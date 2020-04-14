#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "TinyEngine.h"

#include "imgui/imgui_impl_glfw.h"

class Application: public TinyEngine
{
private:

	virtual bool applicationInit();
	virtual bool applicationUpdate(uint32_t frameIndex, double deltaTime, double totalTime);
	virtual void applicationTerminate();

public:
	Application(const char* title);
	~Application();
};

#endif /* APPLICATION_H_ */
