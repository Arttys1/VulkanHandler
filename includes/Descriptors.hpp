#ifndef VK_DESCRIPTOR_HPP
#define VK_DESCRIPTOR_HPP

#include <vulkan/vulkan.h>
#include <Device.hpp>
#include <vector>

namespace basicvk {
	class DescriptorSetLayout;
	class DescriptorSet;
	class DescriptorPool;

	struct DescriptorPoolCreateInfo {
		std::vector<VkDescriptorPoolSize> poolSizes;
		uint32_t maxSets;
	};

	struct DescriptorSetAllocateInfo {
		VkDescriptorPool descriptorPool;
		VkDescriptorSetLayout descriptorSetLayout;
	};

	struct DescriptorSetLayoutCreateInfo {
		uint32_t binding;
		VkDescriptorType descriptorType;
		VkShaderStageFlags shaderStage;
		uint32_t descriptorCount;
	};

	struct BufferUpdateInfo {
		VkBuffer buffer;
		uint64_t range;
		uint32_t binding;
		uint32_t arrayElement;
		VkDescriptorType type;
	};

	struct TextureUpdateInfo {
		VkImageLayout imageLayout;
		VkImageView imageView;
		VkSampler sampler;
		uint32_t binding;
		uint32_t arrayElement;
	};

	struct DescriptorSetUpdateInfo {
		std::vector<BufferUpdateInfo> bufferInfos;
		std::vector<TextureUpdateInfo> textureInfos;
	};

	class DescriptorPool {
	public:
		DescriptorPool(std::shared_ptr<Device> device, DescriptorPoolCreateInfo createInfo);
		~DescriptorPool();
		DescriptorPool(DescriptorPool& other);
		DescriptorPool operator=(DescriptorPool& other);
		DescriptorPool(DescriptorPool&&) = delete;
		DescriptorPool operator=(DescriptorPool&&) = delete;

		std::shared_ptr<DescriptorSet> allocateDescriptorSet(const DescriptorSetLayout &descriptorSetLayout);
		const std::vector<std::shared_ptr<DescriptorSet>> & getDescriptorSets() const;
		std::vector<VkDescriptorSet> getVkDescriptorSets() const;

	private:
		VkDescriptorPool descriptorPool;
		std::shared_ptr<Device> device_ptr;
		std::vector<std::shared_ptr<DescriptorSet>> descriptorSets;
	};

	class DescriptorSet {
	public:
		DescriptorSet(std::shared_ptr<Device> device, DescriptorSetAllocateInfo allocateInfo);
		DescriptorSet(DescriptorSet& other);
		DescriptorSet operator=(DescriptorSet& other);
		DescriptorSet(DescriptorSet&&) = delete;
		DescriptorSet operator=(DescriptorSet&&) = delete;

		VkDescriptorSet getVkDescriptorSet() const;
		void UpdateDescriptorSet(DescriptorSetUpdateInfo descriptorSetUpdateInfo) const;

	private:
		VkDescriptorSet descriptorSet;
		std::shared_ptr<Device> device_ptr;
	};

	class DescriptorSetLayout {
	public:
		DescriptorSetLayout(std::shared_ptr<Device> device, const std::vector<DescriptorSetLayoutCreateInfo> &createInfo);
		~DescriptorSetLayout();
		DescriptorSetLayout(DescriptorSetLayout& other);
		DescriptorSetLayout operator=(DescriptorSetLayout& other);
		DescriptorSetLayout(DescriptorSetLayout&&) = delete;
		DescriptorSetLayout operator=(DescriptorSetLayout&&) = delete;

		VkDescriptorSetLayout getVkDescriptorSetLayout() const;

	private:
		VkDescriptorSetLayout descriptorSetLayout;
		std::shared_ptr<Device> device_ptr;
	};
}

#endif // !VK_DESCRIPTOR_HPP
