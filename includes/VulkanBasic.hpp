#ifndef VULKAN_BASIC_HPP_
#define VULKAN_BASIC_HPP_

#include <vulkan/vulkan.hpp>

namespace basicvk {
	class VulkanBasic {
	public:
		VulkanBasic(bool windowedApplication = true);
		~VulkanBasic();

		VkInstance getInstance() const;
		VkDebugUtilsMessengerEXT getDebugMenssenger() const;

	private:
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
	};
}


#endif // !VULKAN_BASIC_HPP_
