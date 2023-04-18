#ifndef VK_DEVICE_HPP_
#define VK_DEVICE_HPP_

#include <vulkan/vulkan.hpp>
#include <VulkanBasic.hpp>
#include <PhysicalDevice.hpp>
#include <memory>
#include <optional>

namespace basicvk {
	class Fence;

	class Queue {
	public:
		Queue();
		Queue(VkQueue queue, uint32_t indice);
		Queue(Queue& queue) = default;

		void waitIdle() const;

		VkQueue getVkQueue() const;
		uint32_t getQueueFamilyIndex() const;

	private:
		VkQueue queue;
		uint32_t indice;

	};

	class Device {
	public:
		Device(std::shared_ptr<PhysicalDevice> physicalDevicePtr);
		~Device();
		Device(Device& other);
		Device operator=(Device& other);
		Device(Device&&) = delete;
		Device operator=(Device&&) = delete;

		void waitIdle() const;
		void waitForFences(const Fence &fence, std::uint64_t timeout) const;


		VkDevice getVkDevice() const;
		Queue getGraphicQueue() const;
		Queue getPresentQueue() const;
		std::shared_ptr<PhysicalDevice> getPhysicalDevice() const;

	private:
		VkDevice device;
		std::shared_ptr<PhysicalDevice> physicalDevice;
	};
}


#endif // !VK_DEVICE_HPP_
