#ifndef TINYENGINE_H_
#define TINYENGINE_H_

#include "common/Logger.h"
#include "composite/VulkanRaytraceResource.h"
#include "composite/VulkanResource.h"
#include "io/FileIO.h"
#include "io/ImageDataIO.h"
#include "math/Projection.h"
#include "shader/Compiler.h"
#include "vulkan/HelperVulkan.h"

#include <cstddef>
#include <optional>
#include <vector>

// Volk
#define VK_ENABLE_BETA_EXTENSIONS
#include "volk.h"

// glm
#define GLM_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

// Dear ImGui
#include "gui/imgui/imgui.h"
#include "gui/imgui/vulkan/imgui_impl_vulkan.h"

class TinyEngine
{
private:
	uint32_t major = 1;
	uint32_t minor = 0;
	uint32_t patch = 0;

	std::vector<const char*> enabledInstanceLayerNames = {};
	std::vector<const char*> enabledInstanceExtensionNames = {};

	std::vector<const char*> enabledDeviceExtensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	bool createInstance();
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
	bool createImgui();
	bool createConfiguration();

protected:
	std::string title = "";

	uint32_t width = 0;
	uint32_t height = 0;

	VkFormat colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
	VkFormat depthStencilFormat = VK_FORMAT_UNDEFINED;
	VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
	VkImageUsageFlags imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	VkInstance instance = VK_NULL_HANDLE;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

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
	VkPhysicalDeviceRayTracingPropertiesKHR physicalDeviceRayTracingProperties = {};

	VkPhysicalDeviceFeatures physicalDeviceFeatures = {};
	VkPhysicalDeviceFeatures2 physicalDeviceFeatures2 = {};
	VkPhysicalDeviceRayTracingFeaturesKHR physicalDeviceRayTracingFeatures = {};

	bool useImgui = false;
	VkRenderPass imguiRenderPass = VK_NULL_HANDLE;
	VkDescriptorPool imguiDescriptorPool = VK_NULL_HANDLE;

	bool inResize = false;

	virtual bool applicationInit() = 0;
	virtual bool applicationUpdate(uint32_t frameIndex, double deltaTime, double totalTime) = 0;
	virtual void applicationTerminate() = 0;

public:
	TinyEngine();
	TinyEngine(const char* title);
	virtual ~TinyEngine();

	const char* getTitle() const;

	bool prepare();

	bool init(VkSurfaceKHR surface, uint32_t width, uint32_t height);
	bool resize(uint32_t width, uint32_t height);
	bool update();
	bool terminate();

	//

	VkInstance getInstance() const;

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

	VkFormat getDepthStencilFormat() const;
	void setDepthStencilFormat(VkFormat depthFormat);

	VkSampleCountFlagBits getSamples() const;
	void setSamples(VkSampleCountFlagBits samples);

	VkImageUsageFlags getImageUsage() const;
	void setImageUsage(VkImageUsageFlags imageUsage);

	bool isUseImgui() const;
	void setUseImgui(bool useImgui);

};

#endif /* TINYENGINE_H_ */
