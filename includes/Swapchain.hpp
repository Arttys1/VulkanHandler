#ifndef VK_SWAPCHAIN_HPP_
#define VK_SWAPCHAIN_HPP_

#include <memory>
#include <Device.hpp>
#include <Window.hpp>
#include <Synchronous.hpp>
#include <vulkan/vulkan.hpp>
#include <optional>

namespace basicvk {
	struct SwapchainCreateInfo {
		VkPresentModeKHR presentMode;
		VkSharingMode sharingMode;
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities{};
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class Swapchain {
	public:
		Swapchain(std::shared_ptr<Device> device, const PhysicalDevice &physicalDevice, const Window &window, SwapchainCreateInfo createInfo);
		~Swapchain();
		Swapchain(Swapchain& other);
		Swapchain operator=(Swapchain& other);
		Swapchain(Swapchain&&) = delete;
		Swapchain operator=(Swapchain&&) = delete;

		void acquireNextImage(uint32_t *imageIndex, const Semaphore *pSemaphore, const Fence *pFence, uint64_t timeout) const;
		void presentSwapchain(const Queue& presentQueue, const Semaphore* pSemaphore, uint32_t *imageIndex) const;


		VkFormat getVkSwapChainImageFormat() const;
		VkExtent2D getVkSwapChainExtent() const;
		const std::vector<VkImageView>& getVkSwapchainImageViews() const;

	private:
		std::shared_ptr<Device> device_ptr;
		VkSwapchainKHR swapChain;
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;
	};

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const Window& window);
}

#endif //! VK_SWAPCHAIN_HPP_