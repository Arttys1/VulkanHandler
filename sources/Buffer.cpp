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

		std::shared_ptr<PhysicalDevice> physicalDevice = device_ptr->getPhysicalDevice();
		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice->getVkPhysicalDevice(), &memoryProperties);

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(device_ptr->getVkDevice(), buffer, &memoryRequirements);

		auto properties = (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		uint32_t memoryTypeIndex = physicalDevice->findMemoryType(memoryRequirements.memoryTypeBits, properties);

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


	Texture::Texture(std::shared_ptr<Device> devicePtr, TextureOptions options)
		: image(VK_NULL_HANDLE), imageMemory(VK_NULL_HANDLE), imageView(VK_NULL_HANDLE), sampler(VK_NULL_HANDLE)
		, width(options.width), height(options.height), device_ptr(devicePtr)
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = options.format;
		imageInfo.tiling = options.tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = options.usage;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateImage(device_ptr->getVkDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device_ptr->getVkDevice(), image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = device_ptr->getPhysicalDevice()->findMemoryType(memRequirements.memoryTypeBits, options.properties);

		if (vkAllocateMemory(device_ptr->getVkDevice(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(device_ptr->getVkDevice(), image, imageMemory, 0);

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device_ptr->getVkDevice(), &viewInfo, VK_NULL_HANDLE, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(device_ptr->getPhysicalDevice()->getVkPhysicalDevice(), &properties);
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if (vkCreateSampler(device_ptr->getVkDevice(), &samplerInfo, VK_NULL_HANDLE, &sampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}
	}
	Texture::~Texture()
	{
		if (image != VK_NULL_HANDLE) {
			vkDestroyImage(device_ptr->getVkDevice(), image, VK_NULL_HANDLE);
			image = VK_NULL_HANDLE;
		}
		if (imageMemory != VK_NULL_HANDLE) {
			vkFreeMemory(device_ptr->getVkDevice(), imageMemory, VK_NULL_HANDLE);
			imageMemory = VK_NULL_HANDLE;
		}
		if (imageView != VK_NULL_HANDLE) {
			vkDestroyImageView(device_ptr->getVkDevice(), imageView, VK_NULL_HANDLE);
			imageView = VK_NULL_HANDLE;
		}
		if (sampler != VK_NULL_HANDLE) {
			vkDestroySampler(device_ptr->getVkDevice(), sampler, VK_NULL_HANDLE);
			sampler = VK_NULL_HANDLE;
		}
	}
	Texture::Texture(Texture& other)
		: image(other.image), imageMemory(other.imageMemory), imageView(other.imageView), sampler(other.sampler)
		, width(other.width), height(other.height), device_ptr(other.device_ptr)
	{
		other.image = VK_NULL_HANDLE;
		other.imageMemory = VK_NULL_HANDLE;
		other.imageView = VK_NULL_HANDLE;
		other.sampler = VK_NULL_HANDLE;
		other.width = 0;
		other.height = 0;
	}
	Texture Texture::operator=(Texture& other)
	{
		return Texture(other);
	}
	Texture::Texture(Texture&& other) noexcept
		: image(other.image), imageMemory(other.imageMemory), imageView(other.imageView), sampler(other.sampler)
		, width(other.width), height(other.height), device_ptr(other.device_ptr)
	{
		other.image = VK_NULL_HANDLE;
		other.imageMemory = VK_NULL_HANDLE;
		other.imageView = VK_NULL_HANDLE;
		other.sampler = VK_NULL_HANDLE;
		other.width = 0;
		other.height = 0;
	}
	Texture Texture::operator=(Texture&& other) noexcept
	{
		return Texture(other);
	}
	VkImage Texture::getVkImage() const
	{
		return image;
	}
	VkDeviceMemory Texture::getVkImageMemory() const
	{
		return imageMemory;
	}
	VkImageView Texture::getVkImageView() const
	{
		return imageView;
	}
	VkSampler Texture::getVkSampler() const
	{
		return sampler;
	}
	uint32_t Texture::getWidth() const
	{
		return width;
	}
	uint32_t Texture::getHeight() const
	{
		return height;
	}
}