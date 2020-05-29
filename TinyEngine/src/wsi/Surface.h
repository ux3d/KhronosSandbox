#ifndef WSI_SURFACE_H_
#define WSI_SURFACE_H_

#include "../composite/Composite.h"

#if defined(VK_USE_PLATFORM_WIN32_KHR)

#include <windows.h>

#define NATIVE_DISPLAY             HINSTANCE
#define NATIVE_WINDOW              HWND

#elif defined(VK_USE_PLATFORM_XLIB_KHR)

#include <X11/X.h>

#define NATIVE_DISPLAY             Display*
#define NATIVE_WINDOW              Window

#elif defined(VK_USE_PLATFORM_XCB_KHR)

#include <xcb/xcb.h>

#define NATIVE_DISPLAY             xcb_connection_t*
#define NATIVE_WINDOW              xcb_window_t

#else

#define NATIVE_DISPLAY             void*
#define NATIVE_WINDOW              void*

#endif

class Surface
{
public:

	VkSurfaceKHR create(VkInstance instance, NATIVE_DISPLAY nativeDisplay, NATIVE_WINDOW nativeWindow);

};

#endif /* WSI_SURFACE_H_ */
