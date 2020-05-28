#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "TinyEngine.h"

class Application: public TinyEngine
{
private:

	VkShaderModule vertexShaderModule = VK_NULL_HANDLE;
	VkShaderModule fragmentShaderModule = VK_NULL_HANDLE;

	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline = VK_NULL_HANDLE;

	VertexBufferResource vertexBufferResource = {};

	virtual bool applicationInit();
	virtual bool applicationUpdate(uint32_t frameIndex, double deltaTime, double totalTime);
	virtual void applicationTerminate();

public:
	Application();
	~Application();
};

#endif /* APPLICATION_H_ */
