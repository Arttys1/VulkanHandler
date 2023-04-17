#ifndef VK_FRAMEBUFFER_HPP_
#define VK_FRAMEBUFFER_HPP_

#include <vulkan/vulkan.hpp>
#include <Device.hpp>
#include <Swapchain.hpp>
#include <GraphicPipeline.hpp>
#include <memory>
#include <vector>

namespace basicvk {
	class Framebuffer {
	public:
		Framebuffer(std::shared_ptr<Device> device, const Swapchain& swapchain, const GraphicPipeline &graphicPipeline);
		~Framebuffer();
		Framebuffer(Framebuffer& other);
		Framebuffer operator=(Framebuffer& other);
		Framebuffer(Framebuffer&&) = delete;
		Framebuffer operator=(Framebuffer&&) = delete;

		const std::vector<VkFramebuffer>& getVkSwapchainFramebuffers() const;

	private:
		std::shared_ptr<Device> device_ptr;
		std::vector<VkFramebuffer> swapChainFramebuffers;
	};
 }

#endif // !VK_FRAMEBUFFER_HPP_
