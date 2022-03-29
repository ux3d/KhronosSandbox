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

	std::string filename;
	VkHdrMetadataEXT hdrMetadata{};

	bool imageSrgbNonLinear = false;
	float contentFactor = 1.0f;
	float exposure = 1.0f;
	float maxWhite = 1.0f;
	int32_t tonemap = 0;
	int32_t colorSpace = 0;
	float hdrOffset = 0.0f;
	float hdrScale = 1.0f;
	int32_t transferFunction = 0;
	bool debug = false;

	virtual bool applicationInit();
	virtual bool applicationUpdate(uint32_t frameIndex, double deltaTime, double totalTime);
	virtual void applicationTerminate();

public:
	Application(const std::string& filename, const VkHdrMetadataEXT& hdrMetadata, float contentFactor, float exposure, float maxWhite, int32_t tonemap, bool debug);
	~Application();
};

#endif /* APPLICATION_H_ */
