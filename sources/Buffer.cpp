#include <Buffer.hpp>
#include <cassert>

namespace basicvk {
	Buffer::Buffer(std::shared_ptr<Device> devicePtr, BufferOptions options, uint64_t size)
		: buffer(VK_NULL_HANDLE), bufferMemory(VK_NULL_HANDLE),
		bufferSize(size), device_ptr(devicePtr)
	{
		VkBufferCreateInfo bufferCreateInfo{};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = options.usage;
		bufferCreateInfo.sharingMode = options.sharingMode;

		if (vkCreateBuffer(device_ptr->getVkDevice(), &bufferCreateInfo, VK_NULL_HANDLE, &buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create the buffer");
		}


		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(device_ptr->getPhysicalDevice()->getVkPhysicalDevice(), &memoryProperties);

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(device_ptr->getVkDevice(), buffer, &memoryRequirements);

		uint32_t memoryTypeIndex = 0;
		auto properties = (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
			if ((memoryRequirements.memoryTypeBits & (1 << i)) 
			&& (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				memoryTypeIndex = i;
				break;
			}
		}

		VkMemoryAllocateInfo memoryAllocateInfo{};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

		if (vkAllocateMemory(device_ptr->getVkDevice(), &memoryAllocateInfo, nullptr, &bufferMemory) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to allocate memory to the buffer");
		}

		if (vkBindBufferMemory(device_ptr->getVkDevice(), buffer, bufferMemory, 0) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to bind memory to the buffer");
		}


	}
	Buffer::~Buffer()
	{
		if (buffer != VK_NULL_HANDLE) {
			vkDestroyBuffer(device_ptr->getVkDevice(), buffer, nullptr);
			buffer = VK_NULL_HANDLE;
		}
		if (bufferMemory != VK_NULL_HANDLE) {
			vkFreeMemory(device_ptr->getVkDevice(), bufferMemory, nullptr);
			bufferMemory = VK_NULL_HANDLE;
		}
	}
	uint64_t Buffer::getBufferSize() const
	{
		return bufferSize;
	}
	VkBuffer Buffer::getVkBuffer() const
	{
		return buffer;
	}
	void Buffer::mapMemory(void* data, uint64_t size)
	{
		assert(size <= this->bufferSize);
		void* dest;
		VkResult result = vkMapMemory(device_ptr->getVkDevice(), bufferMemory, 0, size, 0, &dest);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("unable to map memory");
		}

		memcpy(dest, data, static_cast<size_t>(size));
		vkUnmapMemory(device_ptr->getVkDevice(), bufferMemory);
	}
	Buffer::Buffer(Buffer& other) 
		: device_ptr(other.device_ptr), buffer(other.buffer),
		bufferSize(other.bufferSize), bufferMemory(other.bufferMemory)
	{
		other.buffer = VK_NULL_HANDLE;		
		other.bufferMemory = VK_NULL_HANDLE;
		other.bufferSize = 0;
	}
	Buffer::Buffer(Buffer&& other) noexcept
		: device_ptr(other.device_ptr), buffer(other.buffer),
		bufferSize(other.bufferSize), bufferMemory(other.bufferMemory)
	{
		other.buffer = VK_NULL_HANDLE;
		other.bufferMemory = VK_NULL_HANDLE;
		other.bufferSize = 0;
	}
	Buffer Buffer::operator=(Buffer& other)
	{
		return Buffer(other);
	}
	Buffer Buffer::operator=(Buffer&& other) noexcept
	{
		return Buffer(other);
	}
	VkDeviceMemory Buffer::getBufferMemory() const
	{
		return bufferMemory;
	}
}