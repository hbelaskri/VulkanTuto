#include "VertexBuffer.h"
#include "Vertex.h"
#include <cstring>
#include <utility>
#include <tuple>

VertexBuffer::VertexBuffer(Renderer& renderer,
    const std::vector<Vertex>& vertices,
    const std::vector<uint16_t>& indices)
    : renderer(renderer), vertices(vertices), indices(indices)
{
    createVertexBuffer();
    createIndexBuffer();
}

void VertexBuffer::createVertexBuffer()
{
    vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    auto [stagingBuffer, stagingBufferMemory] =
        renderer.createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    void* dataStaging = stagingBufferMemory.mapMemory(0, bufferSize);
    memcpy(dataStaging, vertices.data(), bufferSize);
    stagingBufferMemory.unmapMemory();

    std::tie(vertexBuffer, vertexBufferMemory) =
        renderer.createBuffer(bufferSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);

    copyBuffer(stagingBuffer, vertexBuffer, bufferSize);
}

void VertexBuffer::createIndexBuffer()
{
    vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    auto [stagingBuffer, stagingBufferMemory] =
        renderer.createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    void* data = stagingBufferMemory.mapMemory(0, bufferSize);
    memcpy(data, indices.data(), (size_t)bufferSize);
    stagingBufferMemory.unmapMemory();

    std::tie(indexBuffer, indexBufferMemory) =
        renderer.createBuffer(bufferSize, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);

    copyBuffer(stagingBuffer, indexBuffer, bufferSize);
}

void VertexBuffer::copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size)
{
    vk::CommandPoolCreateInfo poolInfo
    {
    .flags = vk::CommandPoolCreateFlagBits::eTransient,
    .queueFamilyIndex = renderer.GetQueueIndex()
    };

    vk::raii::CommandPool commandPool(renderer.GetDevice(), poolInfo);
    vk::CommandBufferAllocateInfo allocInfo{ .commandPool = commandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1 };
    vk::raii::CommandBuffer commandCopyBuffer = std::move(renderer.GetDevice().allocateCommandBuffers(allocInfo).front());
    commandCopyBuffer.begin({ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
    commandCopyBuffer.copyBuffer(*srcBuffer, *dstBuffer, vk::BufferCopy(0, 0, size));
    commandCopyBuffer.end();

    renderer.GetGraphicsQueue().submit(vk::SubmitInfo{ .commandBufferCount = 1, .pCommandBuffers = &*commandCopyBuffer }, nullptr);
    renderer.GetGraphicsQueue().waitIdle();

}

const vk::raii::Buffer& VertexBuffer::GetVertexBuffer() const
{
    return vertexBuffer;
}

const vk::raii::Buffer& VertexBuffer::GetIndexBuffer() const
{
    return indexBuffer;
}

uint32_t VertexBuffer::GetIndexCount() const
{
    return static_cast<uint32_t>(indices.size());
}