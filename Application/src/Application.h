#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "TinyEngine.h"

class Application: public TinyEngine
{
private:

	RenderManager renderManager;
	GLTF glTF;
	AnimationController animationController;
	std::map<const Node*, uint64_t> nodeToHandles;

	std::string filename = "";
	std::string environment = "";

	float eyeObjectDistance = 5.0f;
	float rotY = 0.0f;
	float rotX = 0.0f;
	float posX = 0.0f;
	float posY = 0.0f;

	float zoomSpeed = 0.05f;

	bool focused = false;

	bool animate = true;
	float worldScale = 1.0f;

	virtual bool applicationInit();
	virtual bool applicationUpdate(uint32_t frameIndex, double deltaTime, double totalTime);
	virtual void applicationTerminate();

public:
	Application(const std::string& filename, const std::string& environment);
	~Application();

	void orbitY(float orbit);
	void orbitX(float orbit);

	void panX(float deltaX);
	void panY(float deltaY);

	void zoom(float zoom);

};

#endif /* APPLICATION_H_ */
