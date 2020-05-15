#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "gltf/GLTF.h"
#include "render/Render.h"
#include "TinyEngine.h"

#include "imgui/imgui_impl_glfw.h"

class Application: public TinyEngine
{
private:

	ResourceManager resourceManager;
	GLTF glTF;

	std::string filename = "";
	std::string environment = "";

	float eyeObjectDistance = 5.0f;
	float rotY = 0.0f;
	float rotX = 0.0f;

	float zoomSpeed = 0.05;

	//

	ImageViewResource raytraceImageViewResource;

	bool raytrace = true;
	uint32_t maxDepth = 0;
	uint32_t specularSamples = 16;
	uint32_t diffuseSamples = 16;

	virtual bool applicationInit();
	virtual bool applicationUpdate(uint32_t frameIndex, double deltaTime, double totalTime);
	virtual void applicationTerminate();

public:
	Application(const char* title, const std::string& filename, const std::string& environment);
	~Application();

	void orbitY(float orbit);
	void orbitX(float orbit);

	void zoom(float zoom);
};

#endif /* APPLICATION_H_ */
