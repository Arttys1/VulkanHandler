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
		VkDescriptorPoolCreateFlags flags;
		uint32_t                    maxSets;
		VkDescriptorType			type;
		uint32_t					descriptorCount;
	};

	struct DescriptorSetAllocateInfo {
		VkDescriptorPool descriptorPool;
		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorType descriptorType;
	};

	struct DescriptorSetLayoutCreateInfo {
		uint32_t binding;
		VkDescriptorType descriptorType;
		VkShaderStageFlags shaderStage;
		uint32_t descriptorCount;
		VkDescriptorSetLayoutCreateFlags flags;
	};

	struct DescriptorSetUpdateInfo {
		VkBuffer buffer;
		uint64_t range;
		uint32_t binding;
		uint32_t arrayElement;
	};

	class DescriptorPool {
	public:
		DescriptorPool(std::shared_ptr<Device> device, DescriptorPoolCreateInfo createInfo);
		~DescriptorPool();
		DescriptorPool(DescriptorPool& other);
		DescriptorPool operator=(DescriptorPool& other);
		DescriptorPool(DescriptorPool&&) = delete;
		DescriptorPool operator=(DescriptorPool&&) = delete;

		std::shared_ptr<DescriptorSet> allocateDescritorSet(const DescriptorSetLayout &descriptorSetLayout);
		const std::vector<std::shared_ptr<DescriptorSet>> & getDescriptorSets() const;
		std::vector<VkDescriptorSet> getVkDescriptorSets() const;

	private:
		VkDescriptorPool descriptorPool;
		VkDescriptorType descriptorType;
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
		VkDescriptorType descriptorType;
		std::shared_ptr<Device> device_ptr;
	};

	class DescriptorSetLayout {
	public:
		DescriptorSetLayout(std::shared_ptr<Device> device, DescriptorSetLayoutCreateInfo createInfo);
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
