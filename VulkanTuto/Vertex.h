#pragma once
#include <glm/glm.hpp>
#include <array>
#include <vulkan/vulkan.hpp>
#include <vector>
#include <cstddef>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;


    static vk::VertexInputBindingDescription getBindingDescription()
    {
        vk::VertexInputBindingDescription bindingDescription{};

        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = vk::VertexInputRate::eVertex;

        return bindingDescription;
    }


    static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions()
    {
        return
        { {
            {
                .location = 0,
                .binding = 0,
                .format = vk::Format::eR32G32B32Sfloat,
                .offset = offsetof(Vertex, pos)
            },
            {
                .location = 1,
                .binding = 0,
                .format = vk::Format::eR32G32B32Sfloat,
                .offset = offsetof(Vertex, color)
            },
            {
                .location = 2,
                .binding = 0,
                .format = vk::Format::eR32G32Sfloat,
                .offset = offsetof(Vertex, texCoord)
            }
        } };
    }
};


const std::vector<Vertex> vertices =
{
    { { -0.5f, -0.5f,  0.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f } },
    { { 0.5f, -0.5f,  0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 0.0f } },
    { { 0.5f,  0.5f,  0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } },
    { { -0.5f,  0.5f,  0.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } },

    { { 0.5f, -0.5f, -1.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f } },
    { { -0.5f, -0.5f, -1.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 0.0f } },
    { { -0.5f,  0.5f, -1.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } },
    { { 0.5f,  0.5f, -1.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } },

    { { -0.5f, 0.5f,  0.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f } },
    { { 0.5f, 0.5f,  0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 0.0f } },
    { { 0.5f, 0.5f, -1.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } },
    { { -0.5f, 0.5f, -1.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } },

    { { -0.5f, -0.5f, -1.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f } },
    { { 0.5f, -0.5f, -1.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 0.0f } },
    { { 0.5f, -0.5f,  0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } },
    { { -0.5f, -0.5f,  0.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } },

    { { -0.5f, -0.5f, -1.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f } },
    { { -0.5f, -0.5f,  0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 0.0f } },
    { { -0.5f,  0.5f,  0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } },
    { { -0.5f,  0.5f, -1.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } },

    { { 0.5f, -0.5f,  0.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f } },
    { { 0.5f, -0.5f, -1.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 0.0f } },
    { { 0.5f,  0.5f, -1.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } },
    { { 0.5f,  0.5f,  0.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } }
};

const std::vector<uint16_t> indices =
{
    0, 1, 2, 2, 3, 0,

    4, 5, 6, 6, 7, 4,

    8, 9, 10, 10, 11, 8,

    12, 13, 14, 14, 15, 12,

    16, 17, 18, 18, 19, 16,

    20, 21, 22, 22, 23, 20
};