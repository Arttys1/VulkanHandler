#ifndef VK_SHADER_HPP_
#define VK_SHADER_HPP_

#include <Device.hpp>
#include <memory>
#include <string>
#include <array>
#include <vulkan/vulkan.hpp>

namespace basicvk {
	class Shader {
	public:
		Shader(std::shared_ptr<Device> device, const std::string &vertexPath, const std::string &fragmentPath);
		~Shader();
		Shader(const Shader &other) = delete;
		Shader(const Shader &&other) = delete;
		Shader operator=(Shader& other) = delete;
		Shader operator=(Shader&&) = delete;

		VkShaderModule getVkFragmentShaderModule() const;
		VkShaderModule getVkVertexShaderModule() const;

		std::array<VkPipelineShaderStageCreateInfo, 2> getPipelineShaderStageCreateInfo() const;
	
	private:
		std::shared_ptr<Device> device_ptr;
		VkShaderModule fragmentShaderModule;
		VkShaderModule vertexShaderModule;
	};
}


#endif //!VK_SHADER_HPP_