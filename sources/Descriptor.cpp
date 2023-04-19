#include <Descriptors.hpp>
#include <vector>

namespace basicvk {
	DescriptorPool::DescriptorPool(std::shared_ptr<Device> device, DescriptorPoolCreateInfo createInfo)
		: device_ptr(device), descriptorPool(VK_NULL_HANDLE), descriptorSets()
	{
		VkDescriptorPoolCreateInfo descriptorCreateInfo{};
		descriptorCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorCreateInfo.maxSets = createInfo.maxSets;
		descriptorCreateInfo.poolSizeCount = createInfo.poolSizes.size();
		descriptorCreateInfo.pPoolSizes = createInfo.poolSizes.data();

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
		: descriptorPool(other.descriptorPool), descriptorSets(other.descriptorSets), device_ptr(other.device_ptr)
	{
		other.descriptorSets.clear();
		other.descriptorPool = VK_NULL_HANDLE;
	}
	DescriptorPool DescriptorPool::operator=(DescriptorPool& other)
	{
		return DescriptorPool(other);
	}

	std::shared_ptr<DescriptorSet> DescriptorPool::allocateDescriptorSet(const DescriptorSetLayout& descriptorSetLayout)
	{
		DescriptorSetAllocateInfo allocateInfo{};
		allocateInfo.descriptorPool = this->descriptorPool;
		allocateInfo.descriptorSetLayout = descriptorSetLayout.getVkDescriptorSetLayout();
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
		: descriptorSet(VK_NULL_HANDLE), device_ptr(device)
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
		: descriptorSet(other.descriptorSet)
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
		std::vector<VkWriteDescriptorSet> writeDescriptorSets;
		std::vector<BufferUpdateInfo>& bufferInfos = descriptorSetUpdateInfo.bufferInfos;
		for (size_t i = 0; i < bufferInfos.size(); i++)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = bufferInfos[i].buffer;
			bufferInfo.offset = 0;
			bufferInfo.range = bufferInfos[i].range;

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSet;
			descriptorWrite.dstBinding = bufferInfos[i].binding;
			descriptorWrite.dstArrayElement = bufferInfos[i].arrayElement;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.descriptorType = bufferInfos[i].type;
			descriptorWrite.pImageInfo = VK_NULL_HANDLE;
			descriptorWrite.pBufferInfo = &bufferInfo;
			descriptorWrite.pTexelBufferView = VK_NULL_HANDLE;
			writeDescriptorSets.push_back(descriptorWrite);
		}

		std::vector<TextureUpdateInfo>& textureInfos = descriptorSetUpdateInfo.textureInfos;
		for (size_t i = 0; i < textureInfos.size(); i++)
		{
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = textureInfos[i].imageView;
			imageInfo.sampler = textureInfos[i].sampler;

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSet;
			descriptorWrite.dstBinding = textureInfos[i].binding;
			descriptorWrite.dstArrayElement = textureInfos[i].arrayElement;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrite.pImageInfo = &imageInfo;
			descriptorWrite.pBufferInfo = VK_NULL_HANDLE;
			descriptorWrite.pTexelBufferView = VK_NULL_HANDLE;
			writeDescriptorSets.push_back(descriptorWrite);
		}

		vkUpdateDescriptorSets(device_ptr->getVkDevice(),
			static_cast<uint32_t>(writeDescriptorSets.size()),
			writeDescriptorSets.data(),
			0, VK_NULL_HANDLE);
	}

	DescriptorSetLayout::DescriptorSetLayout(std::shared_ptr<Device> device, const std::vector<DescriptorSetLayoutCreateInfo> &createInfo)
		: device_ptr(device), descriptorSetLayout(VK_NULL_HANDLE)
	{
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings(createInfo.size());
		for (size_t i = 0; i < createInfo.size(); i++)
		{
			VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};
			descriptorSetLayoutBinding.binding = createInfo[i].binding;
			descriptorSetLayoutBinding.descriptorType = createInfo[i].descriptorType;
			descriptorSetLayoutBinding.descriptorCount = createInfo[i].descriptorCount;
			descriptorSetLayoutBinding.stageFlags = createInfo[i].shaderStage;
			descriptorSetLayoutBinding.pImmutableSamplers = nullptr; // Optional
			layoutBindings[i] = descriptorSetLayoutBinding;
		}

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
		descriptorSetLayoutCreateInfo.pBindings = layoutBindings.data();

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