#include <VulkanBasic.hpp>
#include <ValidationLayer.hpp>
#include <GLFW/glfw3.h>

namespace basicvk {
	VulkanBasic::VulkanBasic(bool windowedApplication)
		: instance(VK_NULL_HANDLE), debugMessenger(VK_NULL_HANDLE)
	{
		std::vector<const char*> extensions;	
		
		if (windowedApplication) {
			if (glfwInit() == GLFW_FALSE) {
				throw std::runtime_error("unable to initialize GLFW");
			}
			uint32_t glfwExtensionCount = 0;
			const char** glfwExtensions;

			glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
			for (uint32_t i = 0; i < glfwExtensionCount; i++) {
				extensions.emplace_back(glfwExtensions[i]);
			}
			
			//extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		}

		if (enableValidationLayers) {
		    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
		//extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.apiVersion = VK_HEADER_VERSION_COMPLETE;

		VkInstanceCreateInfo infoInstance{};
		infoInstance.pApplicationInfo = &appInfo;
		infoInstance.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		infoInstance.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
		infoInstance.enabledLayerCount = 0;
		infoInstance.ppEnabledLayerNames = nullptr;
		infoInstance.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		infoInstance.ppEnabledExtensionNames = extensions.data();
		infoInstance.pNext = nullptr;
		if (enableValidationLayers) {
			infoInstance.ppEnabledLayerNames = validationLayers.data();
			infoInstance.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());

			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
			populateDebugMessengerCreateInfo(debugCreateInfo);
			infoInstance.pNext = &debugCreateInfo;
		}
		
		if (vkCreateInstance(&infoInstance, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("unable to create vkInstance");
		}

		setupDebugMessenger(&instance, &debugMessenger);
	}

	VulkanBasic::~VulkanBasic()
	{
		if (instance != VK_NULL_HANDLE) {
			if (enableValidationLayers && debugMessenger != VK_NULL_HANDLE) {
				DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
				debugMessenger = VK_NULL_HANDLE;
			}
			
			vkDestroyInstance(instance, nullptr);
			instance = VK_NULL_HANDLE;
		}
	}
	VkInstance VulkanBasic::getInstance() const
	{
		return instance;
	}
	VkDebugUtilsMessengerEXT VulkanBasic::getDebugMenssenger() const
	{
		return debugMessenger;
	}
}