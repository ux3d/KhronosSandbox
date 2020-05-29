#include "Surface.h"

VkSurfaceKHR Surface::create(VkInstance instance, NATIVE_DISPLAY nativeDisplay, NATIVE_WINDOW nativeWindow)
{
	VkSurfaceKHR surface = VK_NULL_HANDLE;

	VkResult result = VK_SUCCESS;
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    VkWin32SurfaceCreateInfoKHR win32SurfaceCreateInfoKHR = {};
    win32SurfaceCreateInfoKHR.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    win32SurfaceCreateInfoKHR.hinstance = nativeDisplay;
    win32SurfaceCreateInfoKHR.hwnd = nativeWindow;

    result = vkCreateWin32SurfaceKHR(instance, &win32SurfaceCreateInfoKHR, nullptr, &surface);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
	VkXlibSurfaceCreateInfoKHR xlibSurfaceCreateInfoKHR{};
	xlibSurfaceCreateInfoKHR.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
	xlibSurfaceCreateInfoKHR.dpy = nativeDisplay;
	xlibSurfaceCreateInfoKHR.window = nativeWindow;

    result = vkCreateXlibSurfaceKHR(instance, &xlibSurfaceCreateInfoKHR, nullptr, &surface);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
	VkXcbSurfaceCreateInfoKHR xcbSurfaceCreateInfoKHR = {};
	xcbSurfaceCreateInfoKHR.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
	xcbSurfaceCreateInfoKHR.connection = nativeDisplay;
	xcbSurfaceCreateInfoKHR.window = nativeWindow;

	result = vkCreateXcbSurfaceKHR(instance, &xcbSurfaceCreateInfoKHR, nullptr, &surface);
#endif
	if (result != VK_SUCCESS)
	{
		Logger::print(TinyEngine_ERROR, __FILE__, __LINE__, result);
	}

    return surface;
}
