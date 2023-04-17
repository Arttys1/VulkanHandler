#include <Swapchain.hpp>
#include <cstdint>
#include <limits>
#include <algorithm>

namespace basicvk {
    Swapchain::Swapchain(std::shared_ptr<Device> device, const PhysicalDevice& physicalDevice, const Window& window, SwapchainCreateInfo createInfo)
        : device_ptr(device), swapChain(VK_NULL_HANDLE), swapChainExtent(), swapChainImageFormat()
        , swapChainImages(), swapChainImageViews()
	{
        VkSurfaceKHR surface = window.getVkSurface();
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice.getVkPhysicalDevice(), surface);
        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        swapChainExtent = chooseSwapExtent(swapChainSupport.capabilities, window);
        swapChainImageFormat = surfaceFormat.format;

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }
        QueueFamilyIndices queueFamilyIndices = physicalDevice.getQueueFamillyIndices();
        std::vector<uint32_t> indices{};
        if (queueFamilyIndices.graphicsFamily.has_value()) {
            indices.push_back(queueFamilyIndices.graphicsFamily.value());
        }
        if (queueFamilyIndices.presentFamily.has_value()) {
            indices.push_back(queueFamilyIndices.presentFamily.value());
        }

		VkSwapchainCreateInfoKHR swapchainCreateInfo{};
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = surface;
        swapchainCreateInfo.minImageCount = imageCount;
        swapchainCreateInfo.imageFormat = surfaceFormat.format;
        swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainCreateInfo.imageExtent = swapChainExtent;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.imageSharingMode = createInfo.sharingMode;
        swapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(indices.size());
        swapchainCreateInfo.pQueueFamilyIndices = indices.data();
        swapchainCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = createInfo.presentMode;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
		
        if (vkCreateSwapchainKHR(device->getVkDevice(), &swapchainCreateInfo, VK_NULL_HANDLE, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("unable to create swapchain");
        }

        vkGetSwapchainImagesKHR(device->getVkDevice(), swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        if (vkGetSwapchainImagesKHR(device->getVkDevice(), swapChain, &imageCount, swapChainImages.data()) != VK_SUCCESS) {
            throw std::runtime_error("unable to create swapchain images");  
        }

        swapChainImageViews.resize(swapChainImages.size());
        for (size_t i = 0; i < swapChainImages.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapChainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(device->getVkDevice(), &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create image views!");
            }
        }
	}
    Swapchain::~Swapchain()
    {
        if (swapChain != VK_NULL_HANDLE) {
            vkDestroySwapchainKHR(device_ptr->getVkDevice(), swapChain, VK_NULL_HANDLE);
            swapChain = VK_NULL_HANDLE;
        }

        for (size_t i = 0; i < swapChainImageViews.size(); i++) {
            if (swapChainImageViews[i] != VK_NULL_HANDLE) {
                vkDestroyImageView(device_ptr->getVkDevice(), swapChainImageViews[i], VK_NULL_HANDLE);
                swapChainImageViews[i] = VK_NULL_HANDLE;
            }
        }
    }
    Swapchain::Swapchain(Swapchain& other)
        : device_ptr(other.device_ptr), swapChain(other.swapChain), swapChainImages(other.swapChainImages)
        , swapChainImageFormat(other.swapChainImageFormat), swapChainExtent(other.swapChainExtent)
        , swapChainImageViews(other.swapChainImageViews)
    {
        other.swapChain = VK_NULL_HANDLE;
        other.swapChainImages.clear();
        other.swapChainImageViews.clear();
    }
    Swapchain Swapchain::operator=(Swapchain& other)
    {
        return Swapchain(other);
    }

    void Swapchain::acquireNextImage(
        uint32_t *imageIndex, 
        const Semaphore *pSemaphore, 
        const Fence *pFence, 
        uint64_t timeout) const
    {
        VkSemaphore semaphore = pSemaphore ? pSemaphore->getVkSemaphore() : VK_NULL_HANDLE;
        VkFence fence = pFence ? pFence->getVkFence() : VK_NULL_HANDLE;

        if (vkAcquireNextImageKHR(device_ptr->getVkDevice(), swapChain, timeout
            , semaphore, fence, imageIndex) != VK_SUCCESS) 
        {
            throw std::runtime_error("unable to acquire next image");
        }
    }

    void Swapchain::presentSwapchain(const Queue& presentQueue, const Semaphore* pSemaphore, uint32_t *imageIndex) const
    {
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        if (pSemaphore != nullptr) {
            VkSemaphore vkSemaphore = pSemaphore->getVkSemaphore();
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = &vkSemaphore;
        }
        else {
            presentInfo.waitSemaphoreCount = 0;
            presentInfo.pWaitSemaphores = VK_NULL_HANDLE;
        }
        VkSwapchainKHR swapChains[] = { swapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = imageIndex;

        if (vkQueuePresentKHR(presentQueue.getVkQueue(), &presentInfo) != VK_SUCCESS) {
            throw std::runtime_error("unable to present to the swapchain");
        }
    }

    VkFormat Swapchain::getVkSwapChainImageFormat() const
    {
        return swapChainImageFormat;
    }

    VkExtent2D Swapchain::getVkSwapChainExtent() const
    {
        return swapChainExtent;
    }
    const std::vector<VkImageView>& Swapchain::getVkSwapchainImageViews() const
    {
        return swapChainImageViews;
    }

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        SwapChainSupportDetails details;
        
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }   

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }
        
        return details;
    }
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const Window &window)
    {
        if (capabilities.currentExtent.width != 0xffffffff) {
            return capabilities.currentExtent;
        }
        else {
            int width, height;
            window.getFramebufferSize(&width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }
}