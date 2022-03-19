#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "TinyEngine.h"

class Application: public TinyEngine
{
private:

	VkShaderModule vertexShaderModule = VK_NULL_HANDLE;
	VkShaderModule fragmentShaderModule = VK_NULL_HANDLE;

	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<UniformBufferResource> uniformBufferResources;

	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline = VK_NULL_HANDLE;

	VertexBufferResource vertexBufferResource = {};
	VertexBufferResource vertexIndexBufferResource = {};

	TextureResource textureResource  = {};

	int32_t tonemap = 0;
	int32_t testImage = 0;
	bool srgbIn = false;
	bool debug = false;

	int32_t transferFunction = 0;
	// Display: 		Samsung UE75NU8009 UHD 4K
	// Value:			763.881897 nits (Max Fullframe Value from DisplayHDR Test)
	// Note:			One can read out these values programmatically:
	//					https://www.asawicki.info/news_1703_programming_hdr_monitor_support_in_direct3d
	float monitorMaximumNits = 763.881897f;

	virtual bool applicationInit();
	virtual bool applicationUpdate(uint32_t frameIndex, double deltaTime, double totalTime);
	virtual void applicationTerminate();

public:
	Application(int32_t tonemap, int32_t testImage, bool debug);
	~Application();
};

#endif /* APPLICATION_H_ */
