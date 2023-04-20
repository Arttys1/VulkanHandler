#ifndef VK_GRAPHIC_PIPELINE_HPP_
#define VK_GRAPHIC_PIPELINE_HPP_

#include <vulkan/vulkan.hpp>
#include <Device.hpp>
#include <Swapchain.hpp>
#include <Shader.hpp>
#include <Descriptors.hpp>

namespace basicvk {
	struct GraphicPipelineInfo {
		VkVertexInputBindingDescription* vertexInputBindingDescription;
		std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
		DescriptorSetLayout *descriptorSetLayout;
	};

	struct DepthBuffer {
		VkImage depthImage;
		VkDeviceMemory depthImageMemory;
		VkImageView depthImageView;
	};

	class GraphicPipeline {
	public:
		GraphicPipeline(std::shared_ptr<Device> device, const Swapchain& swapchain, const Shader &shader, GraphicPipelineInfo pipelineInfo);
		~GraphicPipeline();
		GraphicPipeline(GraphicPipeline& other);
		GraphicPipeline operator=(GraphicPipeline& other);
		GraphicPipeline(GraphicPipeline&&) = delete;
		GraphicPipeline operator=(GraphicPipeline&&) = delete;

		VkRenderPass getVkRenderPass() const;
		VkPipeline getVkGraphicPipeline() const;
		VkPipelineLayout getVkPipelineLayout() const;
		DepthBuffer getDepthBuffer() const;

	private:
		std::shared_ptr<Device> device_ptr;
		VkPipeline graphicPipeline;
		VkPipelineLayout pipelineLayout;
		VkRenderPass renderPass;
		DepthBuffer depthBuffer;
	};
}

#endif // !VK_GRAPHIC_PIPELINE_HPP_
