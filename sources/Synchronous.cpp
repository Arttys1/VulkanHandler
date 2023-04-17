#include <Synchronous.hpp>

namespace basicvk {
	Fence::Fence(std::shared_ptr<Device> device, FenceOptions options)
		: fence(VK_NULL_HANDLE), device_ptr(device)
	{
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = options.flags;

		if (vkCreateFence(device->getVkDevice(), &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
			throw std::runtime_error("failed to create Fence");
		}
	}
	Fence::Fence(Fence& other)
		: fence(other.fence), device_ptr(other.device_ptr)
	{
		other.fence = VK_NULL_HANDLE;
	}
	Fence::Fence(Fence&& other)
		: fence(other.fence), device_ptr(other.device_ptr)
	{
		other.fence = VK_NULL_HANDLE;
	}
	Fence Fence::operator=(Fence& other)
	{
		return Fence(other);
	}
	Fence Fence::operator=(Fence&& other)
	{
		return Fence(other);
	}
	Fence::~Fence()
	{
		if (fence != VK_NULL_HANDLE) {
			vkDestroyFence(device_ptr->getVkDevice(), fence, nullptr);
			fence = VK_NULL_HANDLE;
		}
	}
	VkFence Fence::getVkFence() const
	{
		return fence;
	}
	void Fence::wait(std::uint64_t timeout) const
	{
		if (vkWaitForFences(device_ptr->getVkDevice(), 1, &fence, VK_TRUE, timeout) != VK_SUCCESS) {
			throw std::runtime_error("wait for fence failed");
		}
	}

	void Fence::reset() const
	{
		if (vkResetFences(device_ptr->getVkDevice(), 1, &fence) != VK_SUCCESS) {
			throw std::runtime_error("unable to reset fence");
		}
	}


	Semaphore::Semaphore(std::shared_ptr<Device> device)
		: device_ptr(device), semaphore(VK_NULL_HANDLE)
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		if (vkCreateSemaphore(device->getVkDevice(), &semaphoreCreateInfo, VK_NULL_HANDLE, &semaphore) != VK_SUCCESS) {
			throw std::runtime_error("unable to create semaphore");
		}
	}
	Semaphore::~Semaphore()
	{
		if (semaphore != VK_NULL_HANDLE) {
			vkDestroySemaphore(device_ptr->getVkDevice(), semaphore, VK_NULL_HANDLE);
			semaphore = VK_NULL_HANDLE;
		}
	}
	Semaphore::Semaphore(Semaphore& other)
		: device_ptr(other.device_ptr), semaphore(other.semaphore)
	{
		other.semaphore = VK_NULL_HANDLE;
	}
	Semaphore::Semaphore(Semaphore&& other)
		: device_ptr(other.device_ptr), semaphore(other.semaphore)
	{
		other.semaphore = VK_NULL_HANDLE;
	}
	Semaphore Semaphore::operator=(Semaphore& other)
	{
		return Semaphore(other);
	}
	Semaphore Semaphore::operator=(Semaphore&& other)
	{
		return Semaphore(other);
	}
	VkSemaphore Semaphore::getVkSemaphore() const
	{
		return semaphore;
	}
}