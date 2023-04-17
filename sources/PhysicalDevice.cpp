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
}