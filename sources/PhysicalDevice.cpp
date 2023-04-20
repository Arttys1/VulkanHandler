#include <PhysicalDevice.hpp>

namespace basicvk {
	PhysicalDevice::PhysicalDevice(std::shared_ptr<VulkanBasic> basicptr, const Window* pWindow)
		: physicalDevices(), current(0), queueFamilyIndices(), basic(basicptr)
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(basic->getInstance(), &deviceCount, nullptr);

		if (deviceCount == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}

		physicalDevices.resize(deviceCount);
		vkEnumeratePhysicalDevices(basic->getInstance(), &deviceCount, physicalDevices.data());

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[0], &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[0], &queueFamilyCount, queueFamilies.data());
	
		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				queueFamilyIndices.graphicsFamily = i;
			}

			if (pWindow != nullptr) {
				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[0], i, pWindow->getVkSurface(), &presentSupport);
				if (presentSupport) {
					queueFamilyIndices.presentFamily = i;
				}
				i++;
			}
		}

		if (!queueFamilyIndices.graphicsFamily.has_value()) {
			throw std::runtime_error("unable to find graphic queue with required properties");
		}
		if (pWindow != nullptr && !queueFamilyIndices.presentFamily.has_value()) {
			throw std::runtime_error("unable to find present queue with required properties");
		}
	}
	PhysicalDevice::~PhysicalDevice()
	{
	}
	QueueFamilyIndices PhysicalDevice::getQueueFamillyIndices() const
	{
		return queueFamilyIndices;
	}
	VkPhysicalDevice PhysicalDevice::getVkPhysicalDevice() const
	{
		return physicalDevices[current];
	}
	uint32_t PhysicalDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(getVkPhysicalDevice(), &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}
	VkFormat PhysicalDevice::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const {
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(getVkPhysicalDevice(), format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}

		throw std::runtime_error("failed to find supported format!");
	}
	VkFormat PhysicalDevice::findDepthFormat() const
	{
		return findSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}
}