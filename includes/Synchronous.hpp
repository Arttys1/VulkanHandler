#ifndef VK_SYNCHRONOUS_HPP_
#define VK_SYNCHRONOUS_HPP_

#include <vulkan/vulkan.hpp>
#include <Device.hpp>
#include <memory>

namespace basicvk {
	struct FenceOptions {
		VkFenceCreateFlags flags = 0;
	};

	class Fence {
	public:
		Fence(std::shared_ptr<Device> device, FenceOptions options);
		Fence(Fence& other);
		Fence(Fence&&);
		Fence operator=(Fence& other);
		Fence operator=(Fence&&);
		~Fence();

		VkFence getVkFence() const;
		void wait(std::uint64_t timeout) const;
		void reset() const;

	private:
		VkFence fence;
		std::shared_ptr<Device> device_ptr;
	};

	class Semaphore {
	public:
		Semaphore(std::shared_ptr<Device> device);
		~Semaphore();
		Semaphore(Semaphore& other);
		Semaphore(Semaphore&&);
		Semaphore operator=(Semaphore& other);
		Semaphore operator=(Semaphore&&);

		VkSemaphore getVkSemaphore() const;

	private:
		VkSemaphore semaphore;
		std::shared_ptr<Device> device_ptr;
	};
}

#endif // !VK_SYNCHRONOUS_HPP_
