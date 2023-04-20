#include <Framebuffer.hpp>

namespace basicvk {
	Framebuffer::Framebuffer(std::shared_ptr<Device> device, const Swapchain& swapchain, const GraphicPipeline& graphicPipeline)
		: device_ptr(device), swapChainFramebuffers()
	{
		const std::vector<VkImageView>& swapChainImageViews = swapchain.getVkSwapchainImageViews();
		VkExtent2D swapChainExtent = swapchain.getVkSwapChainExtent();
		swapChainFramebuffers.resize(swapChainImageViews.size(), VK_NULL_HANDLE);
		DepthBuffer depthBuffer = graphicPipeline.getDepthBuffer();

		for (size_t i = 0; i < swapChainImageViews.size(); i++) {
			std::array<VkImageView, 2> attachments = {
				swapChainImageViews[i],
				depthBuffer.depthImageView,
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = graphicPipeline.getVkRenderPass();
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(device_ptr->getVkDevice(), &framebufferInfo, VK_NULL_HANDLE, &swapChainFramebuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("unable to create framebuffer!");
			}
		}
	}
	Framebuffer::~Framebuffer()
	{
		for (size_t i = 0; i < swapChainFramebuffers.size(); i++)
		{
			if (swapChainFramebuffers[i] != VK_NULL_HANDLE) {
				vkDestroyFramebuffer(device_ptr->getVkDevice(), swapChainFramebuffers[i], VK_NULL_HANDLE);
				swapChainFramebuffers[i] = VK_NULL_HANDLE;
			}
		}
	}
	Framebuffer::Framebuffer(Framebuffer& other)
		: device_ptr(other.device_ptr), swapChainFramebuffers(other.swapChainFramebuffers)
	{
		other.swapChainFramebuffers.clear();
	}
	Framebuffer Framebuffer::operator=(Framebuffer& other)
	{
		return Framebuffer(other);
	}
	const std::vector<VkFramebuffer>& Framebuffer::getVkSwapchainFramebuffers() const
	{
		return swapChainFramebuffers;
	}
}