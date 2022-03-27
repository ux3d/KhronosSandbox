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

	int32_t colorSpace = 0;
	int32_t tonemap = 0;
	std::string filename;
	bool imageSrgbNonLinear = false;
	VkHdrMetadataEXT hdrMetadata{};
	float maxWhite = 1.0f;
	float exposure = 1.0f;
	float contentFactor = 1.0f;
	bool debug = false;

	int32_t transferFunction = 0;

	virtual bool applicationInit();
	virtual bool applicationUpdate(uint32_t frameIndex, double deltaTime, double totalTime);
	virtual void applicationTerminate();

public:
	Application(int32_t tonemap, const std::string& filename, const VkHdrMetadataEXT& hdrMetadata, float maxWhite, float exposure, float contentFactor, bool debug);
	~Application();
};

#endif /* APPLICATION_H_ */
