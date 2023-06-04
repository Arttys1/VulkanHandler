#ifndef _BASICVK_GLFW_WINDOW_HPP_
#define _BASICVK_GLFW_WINDOW_HPP_

#include <vulkan/vulkan.hpp>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <string>
#include <VulkanBasic.hpp>

namespace basicvk {
	class Window {
	public:
		Window(int width, int heigth, const char *title, std::shared_ptr<VulkanBasic> basic);
		~Window();

		void checkEvent() const;
		bool shouldClose() const;

		const GLFWwindow* getGLFWwindow() const;
		VkSurfaceKHR getVkSurface() const;
		void getFramebufferSize(int* width, int* height) const;
		double getTime() const;

	private:
		GLFWwindow* window;
		VkSurfaceKHR surface;
		std::shared_ptr<VulkanBasic> basic;
	};
}

#endif //! _BASICVK_GLFW_WINDOW_HPP_