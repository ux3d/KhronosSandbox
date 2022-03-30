#ifndef TINYENGINE_H_
#define TINYENGINE_H_

#include "BaseEngine.h"

#include <optional>

#include "math/Math.h"

#include "activity/Activity.h"
#include "io/IO.h"
#include "shader/Shader.h"

#include "composite/Composite.h"
#include "gltf/GLTF.h"

#include "render/Render.h"
#include "wsi/Wsi.h"

#include "builder/Builder.h"

class TinyEngine : public BaseEngine
{
private:
	bool choosePhysicalDevice();
	bool createSurface();
	bool createDevice();
	bool createSwapchain();
	bool createColorResources();
	bool createDepthStencilResources();
	bool createRenderpass();
	bool createFramebuffer();
	bool createCommandResources();
	bool createSynchronizationResources();
	bool createConfiguration();

protected:
	uint32_t width = 0;
	uint32_t height = 0;

	VkFormat colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
	VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	VkFormat depthStencilFormat = VK_FORMAT_UNDEFINED;
	VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
	VkImageUsageFlags imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	VkSurfaceKHR surface = VK_NULL_HANDLE;
	uint32_t minImageCount = 0;
	VkSurfaceFormatKHR surfaceFormat = {};
	VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

	VkDevice device = VK_NULL_HANDLE;
	std::optional<uint32_t> queueFamilyIndex;
	VkQueue queue = VK_NULL_HANDLE;

	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	std::vector<VkImage> swapchainImages;
	std::vector<VkImageView> swapchainImageViews;
	uint32_t frameIndex = 0;

	ImageViewResource msaa;

	ImageViewResource depth;

	VkRenderPass renderPass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> framebuffers;

	VkCommandPool commandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> queueSubmitFences;

	VkPhysicalDeviceProperties physicalDeviceProperties = {};
	VkPhysicalDeviceProperties2 physicalDeviceProperties2 = {};

	VkPhysicalDeviceFeatures physicalDeviceFeatures = {};
	VkPhysicalDeviceFeatures2 physicalDeviceFeatures2 = {};

	virtual bool applicationInit() = 0;
	virtual bool applicationUpdate(uint32_t frameIndex, double deltaTime, double totalTime) = 0;
	virtual void applicationTerminate() = 0;

public:
	TinyEngine();
	virtual ~TinyEngine();

	bool prepare();

	bool init(VkSurfaceKHR surface);
	bool resize();
	bool update();
	bool terminate();

	//

	VkInstance getInstance() const;
	VkPhysicalDevice getPhysicalDevice() const;
	VkDevice getDevice() const;
	uint32_t getQueueFamilyIndex() const;

	const std::string& getApplicationName() const;
	void setApplicationName(const std::string& applicationName);

	uint32_t getMajor() const;
	void setMajor(uint32_t major);
	uint32_t getMinor() const;
	void setMinor(uint32_t minor);
	uint32_t getPatch() const;
	void setPatch(uint32_t patch);

	void addEnabledInstanceLayerName(const char* layerName);
	void removeEnabledInstanceLayerName(const char* layerName);

	bool hasEnabledInstanceExtensionName(const char* extensionName) const;
	void addEnabledInstanceExtensionName(const char* extensionName);
	void removeEnabledInstanceExtensionName(const char* extensionName);

	bool hasEnabledDeviceExtensionName(const char* extensionName) const;
	void addEnabledDeviceExtensionName(const char* extensionName);
	void removeEnabledDeviceExtensionName(const char* extensionName);

	VkFormat getColorFormat() const;
	void setColorFormat(VkFormat colorFormat);

	VkColorSpaceKHR getColorSpace() const;
	void setColorSpace(VkColorSpaceKHR colorSpace);

	VkFormat getDepthStencilFormat() const;
	void setDepthStencilFormat(VkFormat depthFormat);

	VkSampleCountFlagBits getSamples() const;
	void setSamples(VkSampleCountFlagBits samples);

	VkImageUsageFlags getImageUsage() const;
	void setImageUsage(VkImageUsageFlags imageUsage);

};

#endif /* TINYENGINE_H_ */
