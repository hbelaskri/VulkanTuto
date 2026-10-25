#pragma once
#include <vector>
#include "Renderer.h"
#include "SwapChain.h"
#include <glm/glm.hpp>
#include "Pipeline.h"

struct UniformBufferObject
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};
class Texture;

class UniformBuffer
{
public:
    UniformBuffer(Renderer& renderer, SwapChain& swapChain, Pipeline& pipeline);
    void updateUniformBuffer(uint32_t currentImage);
    const vk::raii::DescriptorSet& GetDescriptorSet(uint32_t frameIndex) const;
    void createDescriptorSets(Texture& texture);
    void setModelMatrix(const glm::mat4& matrix);

private:
    Renderer& renderer;
    SwapChain& swapChain;
    std::vector<vk::raii::Buffer>       uniformBuffers;
    std::vector<vk::raii::DeviceMemory> uniformBuffersMemory;
    std::vector<void*>                 uniformBuffersMapped;

    void createUniformBuffers();

    vk::raii::DescriptorPool descriptorPool = nullptr;

    void createDescriptorPool();

    Pipeline& pipeline;

    std::vector<vk::raii::DescriptorSet> descriptorSets;
    glm::mat4 modelMatrix{ 1.0f };
};