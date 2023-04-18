#ifndef VK_COMMAND_POOL_HPP_
#define VK_COMMAND_POOL_HPP_

#include <vulkan/vulkan.hpp>
#include <Device.hpp>
#include <memory>
#include <Buffer.hpp>
#include <Swapchain.hpp>
#include <Framebuffer.hpp>
#include <Synchronous.hpp>

namespace basicvk {
	struct CommandBufferUsage {
		VkCommandBufferUsageFlags usage;
	};

	class CommandBuffer {
	public:
		CommandBuffer(std::shared_ptr<Device> device, VkCommandPool vkCommandPool, Queue queue);
		CommandBuffer(CommandBuffer& other);
		CommandBuffer(CommandBuffer&&) = delete;
		CommandBuffer operator=(CommandBuffer& other);
		CommandBuffer operator=(CommandBuffer&&) = delete;
		~CommandBuffer();

		const VkCommandBuffer& getVkCommandBuffer() const;

		void beginCommandBuffer(CommandBufferUsage beginInfo) const;
		void endCommandBuffer() const;
		void resetCommandBuffer() const;

		void CopyBuffer(const Buffer& src, const Buffer& dst) const;
		void CopyBufferToTexture(const Buffer& src, const Texture& dest) const;
		void transitionImageLayout(const Texture& texture, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) const;

		void beginRenderPass(const GraphicPipeline& graphicPipeline, const Swapchain &swapchain, const Framebuffer& frameBuffer, uint32_t indexImage) const;
		void endRenderPass() const;

		void bindGraphicPipeline(const GraphicPipeline& graphicPipeline) const;
		void bindVertexBuffer(const Buffer& vertexBuffer) const;
		void bindIndexBuffer(const Buffer& indexBuffer, VkIndexType indexType) const;
		void bindGraphicDescriptorSet(const GraphicPipeline& graphicPipeline, std::shared_ptr<DescriptorSet> descriptorSet) const;
		void draw(const Swapchain& swapchain, uint32_t vertexCount, uint32_t instanceCount) const;
		void drawIndexed(const Swapchain& swapchain, uint32_t indexCount);
		void QueueSubmit(const std::vector<const Semaphore*> &waitSemaphores, const std::vector<const Semaphore*> &signalSemaphores, const Fence* pFence) const;

	private:
		VkCommandBuffer commandBuffer;
		std::shared_ptr<Device> device_ptr;
		Queue queue;
	};

	class CommandPool {
	public:
		CommandPool(std::shared_ptr<Device> device, Queue queue);
		CommandPool(CommandPool& other);
		CommandPool(CommandPool&&) = delete;
		CommandPool operator=(CommandPool& commandPool);
		CommandPool operator=(CommandPool&&) = delete;
		~CommandPool();

		std::shared_ptr<CommandBuffer> allocateCommandBuffer();
		const std::vector<std::shared_ptr<CommandBuffer>> &getCommandBuffers() const;

	private:
		std::vector<std::shared_ptr<CommandBuffer>> commandBuffers;
		std::shared_ptr<Device> device_ptr;
		VkCommandPool commandPool;
		Queue queue;
	};
}

#endif // !VK_COMMAND_POOL_HPP_
