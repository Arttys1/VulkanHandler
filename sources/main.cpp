#include <iostream>
#include <thread>
#include <VulkanBasic.hpp>
#include <PhysicalDevice.hpp>
#include <Device.hpp>
#include <Buffer.hpp>
#include <Command.hpp>
#include <Synchronous.hpp>
#include <Window.hpp>
#include <Descriptors.hpp>
#include <Swapchain.hpp>
#include <Shader.hpp>
#include <GraphicPipeline.hpp>
#include <Framebuffer.hpp>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "../third_party/stb_image.h"

const int MAX_FRAMES_IN_FLIGHT = 2;

int main()
{
    std::shared_ptr<basicvk::VulkanBasic> basicptr = std::make_shared<basicvk::VulkanBasic>(true);
    basicvk::Window window(1000, 800, "ho ! it works :D", basicptr);

    std::shared_ptr<basicvk::PhysicalDevice> physicalDevice = std::make_shared< basicvk::PhysicalDevice>(basicptr, &window);
    std::shared_ptr<basicvk::Device> device = std::make_shared<basicvk::Device>(physicalDevice);
    basicvk::Queue graphicQueue = device->getGraphicQueue();
    basicvk::Queue presentQueue = device->getPresentQueue();
    basicvk::CommandPool commandPool(device, graphicQueue);

    basicvk::SwapchainCreateInfo swapchainCreateInfo{};
    swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapchainCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    basicvk::Swapchain swapchain(device, *physicalDevice, window, swapchainCreateInfo);

    basicvk::Shader shader(device, "../../../shaders/vert.spv", "../../../shaders/frag.spv");


    /////STRUCT AND CONSTANT

    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;
    };

    struct UniformBufferObject {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
    };

    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4
    };

    //BUFFER

    basicvk::BufferOptions vertexBufferOption{};
    vertexBufferOption.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vertexBufferOption.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    basicvk::Buffer vertexBuffer(device, vertexBufferOption, (vertices.size() * sizeof(Vertex)));
    vertexBuffer.mapMemory((void*)vertices.data(), vertices.size() * sizeof(Vertex));

    basicvk::BufferOptions indexBufferOption{};
    indexBufferOption.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    indexBufferOption.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    basicvk::Buffer indexBuffer(device, indexBufferOption, (indices.size() * sizeof(uint16_t)));
    indexBuffer.mapMemory((void*)indices.data(), indices.size() * sizeof(uint16_t));

    //GRAPHIC PIPELINE

    basicvk::GraphicPipelineInfo graphicPipelineInfo{};
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    graphicPipelineInfo.vertexInputBindingDescription = &bindingDescription;

    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);
    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

    graphicPipelineInfo.vertexInputAttributeDescriptions = attributeDescriptions;

    //descriptor
    basicvk::DescriptorSetLayoutCreateInfo uboLayoutCreateInfo{};
    uboLayoutCreateInfo.binding = 0;
    uboLayoutCreateInfo.descriptorCount = 1;
    uboLayoutCreateInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutCreateInfo.shaderStage = VK_SHADER_STAGE_VERTEX_BIT;

    basicvk::DescriptorSetLayoutCreateInfo imageSamplerLayoutCreateInfo{};
    imageSamplerLayoutCreateInfo.binding = 1;
    imageSamplerLayoutCreateInfo.descriptorCount = 1;
    imageSamplerLayoutCreateInfo.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    imageSamplerLayoutCreateInfo.shaderStage = VK_SHADER_STAGE_FRAGMENT_BIT;

    basicvk::DescriptorSetLayout descriptorSetLayout(device, { uboLayoutCreateInfo, imageSamplerLayoutCreateInfo });

    graphicPipelineInfo.descriptorSetLayout = &descriptorSetLayout;
    basicvk::GraphicPipeline graphicPipeline(device, swapchain, shader, graphicPipelineInfo);
    basicvk::Framebuffer framebuffer(device, swapchain, graphicPipeline);

    std::vector<VkDescriptorPoolSize> poolSizes(2);
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    basicvk::DescriptorPoolCreateInfo descriptorPoolCreateInfo{};
    descriptorPoolCreateInfo.poolSizes = poolSizes;
    descriptorPoolCreateInfo.maxSets = MAX_FRAMES_IN_FLIGHT;
    basicvk::DescriptorPool descriptorPool(device, descriptorPoolCreateInfo);


    //Prepare to render

    std::vector<basicvk::Semaphore> imageAvailableSemaphores;
    std::vector<basicvk::Semaphore> renderFinishedSemaphores;
    std::vector<basicvk::Fence> inFlightFences;
    std::vector<basicvk::Buffer> uniformBuffers;

    VkExtent2D swapChainExtent = swapchain.getVkSwapChainExtent();
    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), 10.0f * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        basicvk::BufferOptions uniformBufferOption{};
        uniformBufferOption.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        uniformBufferOption.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        basicvk::Buffer uniformBuffer(device, uniformBufferOption, sizeof(UniformBufferObject));
        uniformBuffer.mapMemory((void*) &ubo, sizeof(UniformBufferObject));
        uniformBuffers.push_back(basicvk::Buffer(uniformBuffer));

        descriptorPool.allocateDescriptorSet(descriptorSetLayout);
        commandPool.allocateCommandBuffer();
        imageAvailableSemaphores.push_back(basicvk::Semaphore(device));
        renderFinishedSemaphores.push_back(basicvk::Semaphore(device));
        basicvk::FenceOptions fenceOptions{};
        fenceOptions.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        inFlightFences.push_back(basicvk::Fence(device, fenceOptions));
    }

    /// Creation de la texture


    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("C:/Users/Arnaud/Downloads/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    basicvk::BufferOptions imageBufferOption{};
    imageBufferOption.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    imageBufferOption.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    basicvk::Buffer imageBuffer(device, imageBufferOption, imageSize);
    imageBuffer.mapMemory(pixels, imageSize);

    basicvk::TextureOptions options{};
    options.height = texHeight;
    options.width = texWidth;
    options.format = VK_FORMAT_R8G8B8A8_SRGB;
    options.tiling = VK_IMAGE_TILING_OPTIMAL;
    options.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    options.usage = (VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
    options.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    options.useMimaping = true;
    basicvk::Texture texture(device, options);

    auto imageCommandBuffer = commandPool.allocateCommandBuffer();
    
    imageCommandBuffer->beginCommandBuffer({ VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT });
    imageCommandBuffer->transitionImageLayout(texture, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    imageCommandBuffer->CopyBufferToTexture(imageBuffer, texture);
    imageCommandBuffer->generateMipMap(texture);
    imageCommandBuffer->endCommandBuffer();
    imageCommandBuffer->QueueSubmit({}, {}, nullptr);
    graphicQueue.waitIdle();

    const std::vector<std::shared_ptr<basicvk::CommandBuffer>> &commandBuffers = commandPool.getCommandBuffers();
    const std::vector<std::shared_ptr<basicvk::DescriptorSet>> &descriptorSets = descriptorPool.getDescriptorSets();

    for (size_t i = 0; i < descriptorSets.size(); i++)
    {
        basicvk::BufferUpdateInfo bufferUpdateInfo{};
        bufferUpdateInfo.arrayElement = 0;
        bufferUpdateInfo.binding = 0;
        bufferUpdateInfo.buffer = uniformBuffers[i].getVkBuffer();
        bufferUpdateInfo.range = sizeof(UniformBufferObject);
        bufferUpdateInfo.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

        basicvk::TextureUpdateInfo textureToUpdate{};
        textureToUpdate.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        textureToUpdate.imageView = texture.getVkImageView();
        textureToUpdate.sampler = texture.getVkSampler();
        textureToUpdate.binding = 1;
        textureToUpdate.arrayElement = 0;

        basicvk::DescriptorSetUpdateInfo descriptorSetUpdateInfo{};
        descriptorSetUpdateInfo.bufferInfos = { bufferUpdateInfo };
        descriptorSetUpdateInfo.textureInfos = { textureToUpdate };

        descriptorSets[i]->UpdateDescriptorSet(descriptorSetUpdateInfo);
    }

    ///////RENDER

    uint32_t currentFrame = 0;
    while (!window.shouldClose()) {
        window.checkEvent();

        const auto& commandBuffer = commandBuffers[currentFrame];
        const auto& inFlightFence = inFlightFences[currentFrame];
        const auto& imageAvailableSemaphore = imageAvailableSemaphores[currentFrame];
        const auto& renderFinishedSemaphore = imageAvailableSemaphores[currentFrame];

        device->waitForFences(inFlightFence, UINT64_MAX);        
        inFlightFence.reset();

        uint32_t imageIndex;
        swapchain.acquireNextImage(&imageIndex, &imageAvailableSemaphore, nullptr, UINT64_MAX);

        commandBuffer->resetCommandBuffer();
        basicvk::CommandBufferUsage usage{};
        usage.usage = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        commandBuffer->beginCommandBuffer(usage);
        commandBuffer->beginRenderPass(graphicPipeline, swapchain, framebuffer, imageIndex);
        commandBuffer->bindGraphicPipeline(graphicPipeline);
        commandBuffer->bindVertexBuffer(vertexBuffer);
        commandBuffer->bindIndexBuffer(indexBuffer, VK_INDEX_TYPE_UINT16);
        commandBuffer->bindGraphicDescriptorSet(graphicPipeline, descriptorSets[currentFrame]);
        commandBuffer->drawIndexed(swapchain, static_cast<uint32_t>(indices.size()));
        commandBuffer->endRenderPass();
        commandBuffer->endCommandBuffer();

        commandBuffer->QueueSubmit({ &imageAvailableSemaphore }, { &renderFinishedSemaphore }, &inFlightFence);

        swapchain.presentSwapchain(presentQueue, &renderFinishedSemaphore, &imageIndex);

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    device->waitIdle();

    std::cout << "everything seems to work properly" << std::endl;
	return 0;
}
