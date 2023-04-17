#include "Command.hpp"

namespace basicvk {
	CommandPool::CommandPool(std::shared_ptr<Device> device, Queue queue)
		: commandBuffers(), commandPool(VK_NULL_HANDLE), device_ptr(device), queue(queue)
	{
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queue.getQueueFamilyIndex();
		poolInfo.pNext = nullptr;

		if (vkCreateCommandPool(device->getVkDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create command pool!");
		}
	}
	CommandPool::~CommandPool()
	{
		if (commandPool != VK_NULL_HANDLE) {
			vkDestroyCommandPool(device_ptr->getVkDevice(), commandPool, nullptr);
			commandPool = VK_NULL_HANDLE;
		}
	}
	CommandPool::CommandPool(CommandPool& other)
		: commandBuffers(other.commandBuffers), commandPool(other.commandPool), device_ptr(other.device_ptr), queue(other.queue)
	{
		other.commandBuffers.clear();
		other.commandPool = VK_NULL_HANDLE;
	}
	CommandPool CommandPool::operator=(CommandPool& commandPool)
	{
		return CommandPool(commandPool);
	}
	std::shared_ptr<CommandBuffer> CommandPool::allocateCommandBuffer()
	{
		std::shared_ptr<CommandBuffer> commandBuffer = std::make_shared<CommandBuffer>(device_ptr, commandPool, queue);
		commandBuffers.push_back(commandBuffer);
		return commandBuffer;

	}
	const std::vector<std::shared_ptr<CommandBuffer>>& CommandPool::getCommandBuffers() const
	{
		return commandBuffers;
	}
	CommandBuffer::CommandBuffer(std::shared_ptr<Device> device, VkCommandPool vkCommandPool, Queue queue)
		: commandBuffer(VK_NULL_HANDLE), device_ptr(device), queue(queue)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = vkCommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(device->getVkDevice(), &allocInfo, &commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}
	CommandBuffer::CommandBuffer(CommandBuffer& other)
		: commandBuffer(other.commandBuffer), device_ptr(other.device_ptr)
	{
		other.commandBuffer = VK_NULL_HANDLE;
	}
	CommandBuffer CommandBuffer::operator=(CommandBuffer& other)
	{
		return CommandBuffer(other);
	}
	CommandBuffer::~CommandBuffer()
	{
	}
	const VkCommandBuffer& CommandBuffer::getVkCommandBuffer() const
	{
		return commandBuffer;
	}
	void CommandBuffer::beginCommandBuffer(CommandBufferUsage info) const
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = info.usage;
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}
	}
	void CommandBuffer::endCommandBuffer() const
	{
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to end command buffer!");
		}
	}
	void CommandBuffer::resetCommandBuffer() const
	{
		if (vkResetCommandBuffer(commandBuffer, 0) != VK_SUCCESS) {
			throw std::runtime_error("unable to reset command buffer");
		}
	}
	void CommandBuffer::CopyBuffer(const Buffer& src, const Buffer& dst) const
	{
		VkBufferCopy  bufferCopyInfo{};
		bufferCopyInfo.dstOffset = 0;
		bufferCopyInfo.srcOffset = 0;
		bufferCopyInfo.size = src.getBufferSize() <= dst.getBufferSize() ? src.getBufferSize() : dst.getBufferSize();
		
		vkCmdCopyBuffer(commandBuffer, src.getVkBuffer(), dst.getVkBuffer(), 1, &bufferCopyInfo);
	}
	void CommandBuffer::beginRenderPass(const GraphicPipeline &graphicPipeline, const Swapchain& swapchain, const Framebuffer &frameBuffer, uint32_t indexImage) const
	{
		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = graphicPipeline.getVkRenderPass();
		renderPassBeginInfo.framebuffer = frameBuffer.getVkSwapchainFramebuffers()[indexImage];
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = swapchain.getVkSwapChainExtent();
		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}
	void CommandBuffer::endRenderPass() const
	{
		vkCmdEndRenderPass(commandBuffer);
	}
	void CommandBuffer::bindGraphicPipeline(const GraphicPipeline& graphicPipeline) const
	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicPipeline.getVkGraphicPipeline());
	}
	void CommandBuffer::bindVertexBuffer(const Buffer& vertexBuffer) const
	{
		VkBuffer vertexBuffers[] = { vertexBuffer.getVkBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	}
	void CommandBuffer::bindIndexBuffer(const Buffer& indexBuffer, VkIndexType indexType) const
	{
		vkCmdBindIndexBuffer(commandBuffer, indexBuffer.getVkBuffer(), 0, indexType);
	}
	void CommandBuffer::bindGraphicDescriptorSet(const GraphicPipeline& graphicPipeline, std::shared_ptr<DescriptorSet> descriptorSet) const
	{
		VkDescriptorSet vkDescriptorSet = descriptorSet->getVkDescriptorSet();
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicPipeline.getVkPipelineLayout(), 0, 1, &vkDescriptorSet, 0, VK_NULL_HANDLE);
	}
	void CommandBuffer::draw(const Swapchain& swapchain, uint32_t vertexCount, uint32_t instanceCount) const
	{
		VkExtent2D swapChainExtent = swapchain.getVkSwapChainExtent();
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapChainExtent.width);
		viewport.height = static_cast<float>(swapChainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapChainExtent;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdDraw(commandBuffer, vertexCount, instanceCount, 0, 0);
	}
	void CommandBuffer::drawIndexed(const Swapchain& swapchain, uint32_t indexCount)
	{
		VkExtent2D swapChainExtent = swapchain.getVkSwapChainExtent();
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapChainExtent.width);
		viewport.height = static_cast<float>(swapChainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapChainExtent;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
	}
	void CommandBuffer::QueueSubmit(const Semaphore &waitSemaphore, const Semaphore& signalSemaphore, const Fence *pFence) const
	{
		//todo : allow many semaphore to be submited at the same time
		VkSemaphore waitSemaphores[] = { waitSemaphore.getVkSemaphore() };
		VkSemaphore signalSemaphores[] = { signalSemaphore.getVkSemaphore() };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		VkFence fence = pFence ? pFence->getVkFence() : VK_NULL_HANDLE;
		if (vkQueueSubmit(queue.getVkQueue(), 1, &submitInfo, fence) != VK_SUCCESS) {
			throw std::runtime_error("unable to submit draw command buffer!");
		}
	}
}