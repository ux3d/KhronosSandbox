#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "TinyEngine.h"

class Application: public TinyEngine
{
private:

	VertexBufferResource vertexBufferResource = {};
	VertexBufferResource vertexIndexBufferResource = {};

	BottomLevelResource bottomLevelResource = {};

	//

	BufferResource accelerationStructureInstanceBuffer = {};

	TopLevelResource topLevelResource = {};

	//

	ImageViewResource raytraceImageViewResource;

	UniformBufferResource uniformBufferResource;

	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

	VkShaderModule rayGenShaderModule = VK_NULL_HANDLE;
	VkShaderModule missShaderModule = VK_NULL_HANDLE;
	VkShaderModule closestHitShaderModule = VK_NULL_HANDLE;

	VkPipeline pipeline = VK_NULL_HANDLE;

	BufferResource shaderBindingBufferResource = {};

	virtual bool applicationInit();
	virtual bool applicationUpdate(uint32_t frameIndex, double deltaTime, double totalTime);
	virtual void applicationTerminate();

public:
	Application(const char* title);
	~Application();
};

#endif /* APPLICATION_H_ */
