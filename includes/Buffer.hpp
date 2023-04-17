#ifndef VK_BUFFER_HPP
#define VK_BUFFER_HPP

#include <vulkan/vulkan.hpp>
#include <Device.hpp>
#include <memory>

namespace basicvk {
	struct BufferOptions {
		VkBufferUsageFlags usage;
		VkSharingMode sharingMode;
	};

	class Buffer {
	public:
		Buffer(std::shared_ptr<Device> devicePtr, BufferOptions options, uint64_t size);
		~Buffer();
		Buffer(Buffer& buffer);
		Buffer operator=(Buffer& other);
		Buffer(Buffer&& other) noexcept;
		Buffer operator=(Buffer&& other) noexcept;

		VkBuffer getVkBuffer() const;
		uint64_t getBufferSize() const;
		VkDeviceMemory getBufferMemory() const;

		void mapMemory(void* data, uint64_t size);

	private:
		VkBuffer buffer;
		VkDeviceMemory bufferMemory;
		uint64_t bufferSize;
		std::shared_ptr<Device> device_ptr;
	};
}


#endif // !VK_BUFFER_HPP
