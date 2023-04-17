#ifndef VK_PHYSICAL_DEVICE_HPP_
#define VK_PHYSICAL_DEVICE_HPP_

#include <vulkan/vulkan.hpp>
#include <VulkanBasic.hpp>
#include <Window.hpp>
#include <vector>
#include <optional>

namespace basicvk {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
	};

	class PhysicalDevice {
	public:
		PhysicalDevice(std::shared_ptr<VulkanBasic> basicptr, const Window *pWindow);
		~PhysicalDevice();

		QueueFamilyIndices getQueueFamillyIndices() const;
		VkPhysicalDevice getVkPhysicalDevice() const;

	private:
		std::vector<VkPhysicalDevice> physicalDevices;
		std::uint8_t current;
		QueueFamilyIndices queueFamilyIndices;
		std::shared_ptr<VulkanBasic> basic;
	};
}

#endif // !VK_PHYSICAL_DEVICE_HPP_
