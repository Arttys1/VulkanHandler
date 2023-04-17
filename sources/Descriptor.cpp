#include <Descriptors.hpp>
#include <vector>

namespace basicvk {
	DescriptorPool::DescriptorPool(std::shared_ptr<Device> device, DescriptorPoolCreateInfo createInfo)
		: device_ptr(device), descriptorPool(VK_NULL_HANDLE), descriptorType(createInfo.type),descriptorSets()
	{
		VkDescriptorPoolSize poolSize{};
		poolSize.descriptorCount = createInfo.descriptorCount;
		poolSize.type = createInfo.type;

		VkDescriptorPoolCreateInfo descriptorCreateInfo{};
		descriptorCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorCreateInfo.flags = createInfo.flags;
		descriptorCreateInfo.maxSets = createInfo.maxSets;
		descriptorCreateInfo.poolSizeCount = 1;
		descriptorCreateInfo.pPoolSizes = &poolSize;

		if (vkCreateDescriptorPool(device->getVkDevice(), &descriptorCreateInfo, VK_NULL_HANDLE, &this->descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}
	DescriptorPool::~DescriptorPool()
	{
		if (descriptorPool != VK_NULL_HANDLE) {
			vkDestroyDescriptorPool(device_ptr->getVkDevice(), descriptorPool, VK_NULL_HANDLE);
			descriptorPool = VK_NULL_HANDLE;
		}
	}
	DescriptorPool::DescriptorPool(DescriptorPool& other)
		: descriptorPool(other.descriptorPool),descriptorType(other.descriptorType), descriptorSets(other.descriptorSets), device_ptr(other.device_ptr)
	{
		other.descriptorSets.clear();
		other.descriptorPool = VK_NULL_HANDLE;
	}
	DescriptorPool DescriptorPool::operator=(DescriptorPool& other)
	{
		return DescriptorPool(other);
	}

	std::shared_ptr<DescriptorSet> DescriptorPool::allocateDescritorSet(const DescriptorSetLayout& descriptorSetLayout)
	{
		DescriptorSetAllocateInfo allocateInfo{};
		allocateInfo.descriptorPool = this->descriptorPool;
		allocateInfo.descriptorSetLayout = descriptorSetLayout.getVkDescriptorSetLayout();
		allocateInfo.descriptorType = descriptorType;
		std::shared_ptr<DescriptorSet> descriptorSet = std::make_shared<DescriptorSet>(device_ptr, allocateInfo);
		descriptorSets.push_back(descriptorSet);
		return descriptorSet;
	}

	const std::vector<std::shared_ptr<DescriptorSet>>& DescriptorPool::getDescriptorSets() const
	{
		return descriptorSets;
	}

	std::vector<VkDescriptorSet> DescriptorPool::getVkDescriptorSets() const
	{
		std::vector<VkDescriptorSet> res(descriptorSets.size(), VK_NULL_HANDLE);
		
		for (auto& descriptorSet : descriptorSets) {
			res.emplace_back(descriptorSet->getVkDescriptorSet());
		}

		return res;
	}

	DescriptorSet::DescriptorSet(std::shared_ptr<Device> device, DescriptorSetAllocateInfo allocateInfo)
		: descriptorSet(VK_NULL_HANDLE), descriptorType(allocateInfo.descriptorType), device_ptr(device)
	{
		VkDescriptorSetLayout descriptorSetLayout = allocateInfo.descriptorSetLayout;
		VkDescriptorSetAllocateInfo descriptorAllocateInfo{};
		descriptorAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorAllocateInfo.descriptorPool = allocateInfo.descriptorPool;
		descriptorAllocateInfo.descriptorSetCount = 1;
		descriptorAllocateInfo.pSetLayouts = &descriptorSetLayout;

		if (vkAllocateDescriptorSets(device->getVkDevice(), &descriptorAllocateInfo, &descriptorSet) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptorSet");
		}
	}

	DescriptorSet::DescriptorSet(DescriptorSet& other)
		: descriptorSet(other.descriptorSet), descriptorType(other.descriptorType)
	{
		other.descriptorSet = VK_NULL_HANDLE;
	}

	DescriptorSet DescriptorSet::operator=(DescriptorSet& other)
	{
		return DescriptorSet(other);
	}

	VkDescriptorSet DescriptorSet::getVkDescriptorSet() const
	{
		return descriptorSet;
	}

	void DescriptorSet::UpdateDescriptorSet(DescriptorSetUpdateInfo descriptorSetUpdateInfo) const
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = descriptorSetUpdateInfo.buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = descriptorSetUpdateInfo.range;

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSet;
		descriptorWrite.dstBinding = descriptorSetUpdateInfo.binding;
		descriptorWrite.dstArrayElement = descriptorSetUpdateInfo.arrayElement;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.descriptorType = descriptorType;
		descriptorWrite.pImageInfo = VK_NULL_HANDLE;
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pTexelBufferView = VK_NULL_HANDLE;

		vkUpdateDescriptorSets(device_ptr->getVkDevice(), 1, &descriptorWrite, 0, VK_NULL_HANDLE);
	}

	DescriptorSetLayout::DescriptorSetLayout(std::shared_ptr<Device> device, DescriptorSetLayoutCreateInfo createInfo)
		: device_ptr(device), descriptorSetLayout(VK_NULL_HANDLE)
	{
		VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};
		descriptorSetLayoutBinding.binding = createInfo.binding;
		descriptorSetLayoutBinding.descriptorType = createInfo.descriptorType;
		descriptorSetLayoutBinding.descriptorCount = createInfo.descriptorCount;
		descriptorSetLayoutBinding.stageFlags = createInfo.shaderStage;
		descriptorSetLayoutBinding.pImmutableSamplers = nullptr; // Optional

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.flags = createInfo.flags;
		descriptorSetLayoutCreateInfo.bindingCount = 1;
		descriptorSetLayoutCreateInfo.pBindings = &descriptorSetLayoutBinding;

		if (vkCreateDescriptorSetLayout(device->getVkDevice(), &descriptorSetLayoutCreateInfo, VK_NULL_HANDLE, &this->descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout");
		}
	}
	DescriptorSetLayout::~DescriptorSetLayout()
	{
		if (descriptorSetLayout != VK_NULL_HANDLE) {
			vkDestroyDescriptorSetLayout(device_ptr->getVkDevice(), descriptorSetLayout, VK_NULL_HANDLE);
			descriptorSetLayout = VK_NULL_HANDLE;
		}
	}
	DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout& other)
		: device_ptr(other.device_ptr), descriptorSetLayout(other.descriptorSetLayout)
	{
		other.descriptorSetLayout = VK_NULL_HANDLE;
	}
	DescriptorSetLayout DescriptorSetLayout::operator=(DescriptorSetLayout& other)
	{
		return DescriptorSetLayout(other);
	}
	VkDescriptorSetLayout DescriptorSetLayout::getVkDescriptorSetLayout() const
	{
		return descriptorSetLayout;
	}
}