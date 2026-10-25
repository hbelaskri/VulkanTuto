#pragma once
#include <vector>
#include <cstdint>
#include "Renderer.h"
#include "Vertex.h"

class VertexBuffer
{
public:
    VertexBuffer(Renderer& renderer,
    const std::vector<Vertex>& vertices,
    const std::vector<uint16_t>& indices);

    const vk::raii::Buffer& GetVertexBuffer() const;
    const vk::raii::Buffer& GetIndexBuffer() const;
    uint32_t GetIndexCount() const;

private:
    Renderer& renderer;

    const std::vector<Vertex>& vertices;
    const std::vector<uint16_t>& indices;

    vk::raii::Buffer vertexBuffer = nullptr;
    vk::raii::DeviceMemory vertexBufferMemory = nullptr;
    vk::raii::Buffer       indexBuffer = nullptr;
    vk::raii::DeviceMemory indexBufferMemory = nullptr;

    void createVertexBuffer();
    void createIndexBuffer();
    void copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size);

};