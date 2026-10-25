#include "UniformBuffer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cstring>
#include <array>
#include "Texture.h"

UniformBuffer::UniformBuffer(Renderer& renderer, SwapChain& swapChain, Pipeline& pipeline)
    : renderer(renderer), swapChain(swapChain), pipeline(pipeline)
{
    createUniformBuffers();
    createDescriptorPool();
}
void UniformBuffer::updateUniformBuffer(uint32_t currentImage)
{
    UniformBufferObject ubo{};

    ubo.model = modelMatrix;
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), static_cast<float>(swapChain.GetExtent().width) / static_cast<float>(swapChain.GetExtent().height), 0.1f, 10.0f);

    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

const vk::raii::DescriptorSet& UniformBuffer::GetDescriptorSet(uint32_t frameIndex) const
{
    return descriptorSets[frameIndex];
}

void UniformBuffer::createUniformBuffers()
{
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
        auto [buffer, bufferMem] = renderer.createBuffer(
            bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        uniformBuffers.emplace_back(std::move(buffer));
        uniformBuffersMemory.emplace_back(std::move(bufferMem));
        uniformBuffersMapped.emplace_back(uniformBuffersMemory.back().mapMemory(0, bufferSize));
    }
}

void UniformBuffer::createDescriptorPool()
{
    std::array<vk::DescriptorPoolSize, 2> poolSize
    { {
        {
            .type = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = MAX_FRAMES_IN_FLIGHT
        },
        {
            .type = vk::DescriptorType::eCombinedImageSampler,
            .descriptorCount = MAX_FRAMES_IN_FLIGHT
        }
    } };

    vk::DescriptorPoolCreateInfo poolInfo
    {
        .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets = MAX_FRAMES_IN_FLIGHT,
        .poolSizeCount = static_cast<uint32_t>(poolSize.size()),
        .pPoolSizes = poolSize.data()
    };

    descriptorPool = vk::raii::DescriptorPool(renderer.GetDevice(), poolInfo);
}

void UniformBuffer::createDescriptorSets(Texture& texture)
{
    std::vector<vk::DescriptorSetLayout> layouts(
        MAX_FRAMES_IN_FLIGHT,
        *pipeline.GetDescriptorSetLayout()
    );

    vk::DescriptorSetAllocateInfo allocInfo
    {
        .descriptorPool = *descriptorPool,
        .descriptorSetCount = static_cast<uint32_t>(layouts.size()),
        .pSetLayouts = layouts.data()
    };

    descriptorSets = renderer.GetDevice().allocateDescriptorSets(allocInfo);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vk::DescriptorBufferInfo bufferInfo
        {
            .buffer = *uniformBuffers[i],
            .offset = 0,
            .range = sizeof(UniformBufferObject)
        };

        vk::DescriptorImageInfo imageInfo{
            .sampler = *texture.GetTextureSampler(),
            .imageView = *texture.GetTextureImageView(),
            .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal
        };

        std::array<vk::WriteDescriptorSet, 2> descriptorWrites
        { {
         {
             .dstSet = *descriptorSets[i],
             .dstBinding = 0,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = vk::DescriptorType::eUniformBuffer,
             .pBufferInfo = &bufferInfo
         },
         {
             .dstSet = *descriptorSets[i],
             .dstBinding = 1,
             .dstArrayElement = 0,
             .descriptorCount = 1,
             .descriptorType = vk::DescriptorType::eCombinedImageSampler,
             .pImageInfo = &imageInfo
         }
     } };

        renderer.GetDevice().updateDescriptorSets(descriptorWrites, {});
    }
}

void UniformBuffer::setModelMatrix(const glm::mat4& matrix)
{
    modelMatrix = matrix;
}
