#pragma once
#include "Renderer.h"
#include "CommandBuffer.h"
#include <string>

class Texture
{
public:
    Texture(Renderer& renderer, CommandBuffer& commandBuffer,const std::string& texturePath);
    const vk::raii::ImageView& GetTextureImageView() const
    {
        return textureImageView;
    }

    const vk::raii::Sampler& GetTextureSampler() const
    {
        return textureSampler;
    }

private:
    Renderer& renderer;
    CommandBuffer& commandBuffer;
    vk::raii::Image textureImage = nullptr;
    vk::raii::DeviceMemory textureImageMemory = nullptr;
    vk::raii::ImageView textureImageView = nullptr;
    vk::raii::Sampler textureSampler = nullptr;

    void createTextureImage();
    void createTextureImageView();
    void createTextureSampler();

    void transitionImageLayout
    (
        vk::raii::CommandBuffer& commandBuffer,
        const vk::raii::Image& image,
        vk::ImageLayout oldLayout,
        vk::ImageLayout newLayout
    );

    void copyBufferToImage
    (
        vk::raii::CommandBuffer& commandBuffer,
        const vk::raii::Buffer& buffer,
        vk::raii::Image& image,
        uint32_t width,
        uint32_t height
    );
    std::string texturePath;
};