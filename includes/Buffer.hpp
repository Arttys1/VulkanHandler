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

	struct TextureOptions {
		uint32_t width; 
		uint32_t height;
		VkFormat format;
		VkImageTiling tiling;
		VkImageUsageFlags usage;
		VkMemoryPropertyFlags properties;
	};

	class Texture {
	public:
		Texture(std::shared_ptr<Device> devicePtr, TextureOptions options);
		~Texture();
		Texture(Texture& texture);
		Texture operator=(Texture& other);
		Texture(Texture&& other) noexcept;
		Texture operator=(Texture&& other) noexcept;

		VkImage getVkImage() const;
		VkDeviceMemory getVkImageMemory() const;
		VkImageView getVkImageView() const;
		VkSampler getVkSampler() const;
		uint32_t getWidth() const;
		uint32_t getHeight() const;

	private:
		VkImage image;
		VkDeviceMemory imageMemory;
		VkImageView imageView;
		VkSampler sampler;
		uint32_t width;
		uint32_t height;
		std::shared_ptr<Device> device_ptr;
	};
}


#endif // !VK_BUFFER_HPP
