#include <Device.hpp>
#include <Command.hpp>
#include <Synchronous.hpp>
#include <set>

namespace basicvk {
	Device::Device(std::shared_ptr<PhysicalDevice> physicalDevicePtr)
		: device(VK_NULL_HANDLE), physicalDevice(physicalDevicePtr)
	{
		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		QueueFamilyIndices indices = physicalDevicePtr->getQueueFamillyIndices();
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies;
		if (indices.graphicsFamily.has_value()) {
			uniqueQueueFamilies.insert(indices.graphicsFamily.value());
		}
		if (indices.presentFamily.has_value()) {
			uniqueQueueFamilies.insert(indices.presentFamily.value());
		}

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (vkCreateDevice(
			physicalDevicePtr->getVkPhysicalDevice(),
			&createInfo,
			nullptr,
			&device) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create logical device!");
		}
	}
	Device::~Device()
	{
		if (device != VK_NULL_HANDLE) {
			vkDestroyDevice(device, nullptr);
			device = VK_NULL_HANDLE;
		}
	}
	Device::Device(Device& other)
		: physicalDevice(other.physicalDevice), device(other.device)
	{
		other.device = VK_NULL_HANDLE;
	}
	Device Device::operator=(Device& other)
	{
		return Device(other);
	}
	void Device::waitIdle() const
	{
		if (vkDeviceWaitIdle(device) != VK_SUCCESS) {
			throw std::runtime_error("unable to wait idle for this device");
		}
	}
	void Device::waitForFences(const Fence& fence, std::uint64_t timeout) const
	{
		VkFence vkFence = fence.getVkFence();
		vkWaitForFences(device, 1, &vkFence, VK_TRUE, timeout);
	}
	VkDevice Device::getVkDevice() const
	{
		return device;
	}
	Queue Device::getGraphicQueue() const
	{		
		QueueFamilyIndices indices = physicalDevice->getQueueFamillyIndices();
		if (indices.graphicsFamily.has_value()) {
			uint32_t indice = indices.graphicsFamily.value();
			VkQueue graphicQueue;
			vkGetDeviceQueue(device, indice, 0, &graphicQueue);

			return Queue(graphicQueue, indice);
		}
		else {
			return Queue();	//nullptr queue
		}
	}
	Queue Device::getPresentQueue() const
	{
		QueueFamilyIndices indices = physicalDevice->getQueueFamillyIndices();
		if (indices.presentFamily.has_value()) {
			uint32_t indice = indices.presentFamily.value();
			VkQueue presentQueue;
			vkGetDeviceQueue(device, indice, 0, &presentQueue);

			return Queue(presentQueue, indice);
		}
		else {
			return Queue();	//nullptr queue
		}
	}
	std::shared_ptr<PhysicalDevice> Device::getPhysicalDevice() const
	{
		return physicalDevice;
	}
	Queue::Queue()
		: Queue(nullptr, -1)
	{
	}
	Queue::Queue(VkQueue queue, uint32_t indice)
		: queue(queue), indice(indice)
	{
	}
	VkQueue Queue::getVkQueue() const
	{
		return queue;
	}
	uint32_t Queue::getQueueFamilyIndex() const
	{
		return indice;
	}
}