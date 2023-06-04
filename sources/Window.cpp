#include <Window.hpp>
#include <stdexcept>
#include <vulkan/vulkan_win32.h>
#include <cassert>

namespace basicvk {
	Window::Window(int width, int heigth, const char *title, std::shared_ptr<VulkanBasic> basic)
		: window(nullptr), basic(basic)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, heigth, title, nullptr, nullptr);
		if (!window) {
			throw std::runtime_error("unable to create glfw window");
		}

		VkWin32SurfaceCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = glfwGetWin32Window(window);
		createInfo.hinstance = GetModuleHandle(nullptr);

		if (vkCreateWin32SurfaceKHR(basic->getInstance(), &createInfo, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}

	Window::~Window()
	{
		if (window != nullptr) {
			glfwDestroyWindow(window);
			window = nullptr;
			glfwTerminate();
		}
		if (surface != VK_NULL_HANDLE) {
			vkDestroySurfaceKHR(basic->getInstance(), surface, nullptr);
			surface = nullptr;
		}
	}

	void Window::checkEvent() const
	{
		glfwPollEvents();
	}

	bool Window::shouldClose() const
	{
		return glfwWindowShouldClose(window);
	}

	const GLFWwindow * Window::getGLFWwindow() const
	{
		return window;
	}
	VkSurfaceKHR Window::getVkSurface() const
	{
		return surface;
	}
	void Window::getFramebufferSize(int* width, int* height) const
	{
		assert(width != nullptr);
		assert(height != nullptr);
		glfwGetFramebufferSize(window, width, height);
	}
	double Window::getTime() const
	{
		return glfwGetTime();
	}
}
