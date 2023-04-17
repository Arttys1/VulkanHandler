#include <Shader.hpp>
#include <fstream>

namespace basicvk {
	Shader::Shader(std::shared_ptr<Device> device, const std::string& vertexPath, const std::string& fragmentPath)
		: device_ptr(device), fragmentShaderModule(VK_NULL_HANDLE), vertexShaderModule(VK_NULL_HANDLE)
	{
		auto readFile = [](const std::string& path) -> std::vector<char> {
			std::ifstream file(path, std::ios::ate | std::ios::binary);

			if (!file.is_open()) {
				throw std::runtime_error("failed to open file : " + path);
			}
			size_t fileSize = (size_t)file.tellg();
			std::vector<char> buffer(fileSize);
			file.seekg(0);
			file.read(buffer.data(), fileSize);
			file.close();

			return buffer;
		};

		auto createShaderModule = [device, readFile](const std::string& path) -> VkShaderModule {
			std::vector<char> code = readFile(path);

			VkShaderModuleCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = code.size();
			createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

			VkShaderModule shaderModule;
			if (vkCreateShaderModule(device->getVkDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
				throw std::runtime_error("failed to create shader module!");
			}
			return shaderModule;
		};

		this->vertexShaderModule = createShaderModule(vertexPath);
		this->fragmentShaderModule = createShaderModule(fragmentPath);

	}
	Shader::~Shader()
	{
		if (vertexShaderModule != VK_NULL_HANDLE) {
			vkDestroyShaderModule(device_ptr->getVkDevice(), vertexShaderModule, VK_NULL_HANDLE);
			vertexShaderModule = VK_NULL_HANDLE;
		}
		if (fragmentShaderModule != VK_NULL_HANDLE) {
			vkDestroyShaderModule(device_ptr->getVkDevice(), fragmentShaderModule, VK_NULL_HANDLE);
			fragmentShaderModule = VK_NULL_HANDLE;
		}
	}
	VkShaderModule Shader::getVkFragmentShaderModule() const
	{
		return fragmentShaderModule;
	}
	VkShaderModule Shader::getVkVertexShaderModule() const
	{
		return vertexShaderModule;
	}
	std::array<VkPipelineShaderStageCreateInfo, 2> Shader::getPipelineShaderStageCreateInfo() const
	{
		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertexShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragmentShaderModule;
		fragShaderStageInfo.pName = "main";

		std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = { vertShaderStageInfo, fragShaderStageInfo };
		return shaderStages;
	}
}