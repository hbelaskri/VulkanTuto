#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <cstring>
#include <stdexcept>
#include <tuple>

Texture::Texture(Renderer& renderer, CommandBuffer& commandBuffer, const std::string& texturePath)
    : renderer(renderer), commandBuffer(commandBuffer), texturePath(texturePath)

{
    createTextureImage();
    createTextureImageView();
    createTextureSampler();
}

void Texture::transitionImageLayout
(
    vk::raii::CommandBuffer& commandBuffer,
    const vk::raii::Image& image,
    vk::ImageLayout oldLayout,
    vk::ImageLayout newLayout)
{
    vk::ImageMemoryBarrier barrier
    {
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image = *image,
        .subresourceRange =
        {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .levelCount = 1,
            .layerCount = 1
        }
    };

    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    if (oldLayout == vk::ImageLayout::eUndefined &&
        newLayout == vk::ImageLayout::eTransferDstOptimal)
    {
        barrier.srcAccessMask = {};
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (oldLayout == vk::ImageLayout::eTransferDstOptimal &&
        newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        sourceStage = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else
    {
        throw std::invalid_argument("unsupported layout transition!");
    }

    commandBuffer.pipelineBarrier
    (
        sourceStage,
        destinationStage,
        {},
        {},
        nullptr,
        barrier
    );
}
void Texture::copyBufferToImage
(
    vk::raii::CommandBuffer& commandBuffer,
    const vk::raii::Buffer& buffer,
    vk::raii::Image& image,
    uint32_t width,
    uint32_t height)
{
    vk::BufferImageCopy region
    {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = 
        {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1
        },
        .imageOffset = { 0, 0, 0 },
        .imageExtent = { width, height, 1 }
    };

    commandBuffer.copyBufferToImage
    (
        *buffer,
        *image,
        vk::ImageLayout::eTransferDstOptimal,
        region
    );
}

void Texture::createTextureImage()
{
    int texWidth, texHeight, texChannels;

    stbi_uc* pixels = stbi_load
    (
        texturePath.c_str(),
        &texWidth,
        &texHeight,
        &texChannels,
        STBI_rgb_alpha
    );

    if (!pixels)
    {
        throw std::runtime_error("Impossible de charger la texture : " + texturePath);
    }

    vk::DeviceSize imageSize =
        static_cast<vk::DeviceSize>(texWidth) * texHeight * 4;

    auto [stagingBuffer, stagingBufferMemory] =
        renderer.createBuffer(
            imageSize,
            vk::BufferUsageFlagBits::eTransferSrc,
            vk::MemoryPropertyFlagBits::eHostVisible |
            vk::MemoryPropertyFlagBits::eHostCoherent
        );

    void* data = stagingBufferMemory.mapMemory(0, imageSize);

    memcpy(data, pixels, static_cast<size_t>(imageSize));

    stagingBufferMemory.unmapMemory();

    stbi_image_free(pixels);

    std::tie(textureImage, textureImageMemory) = renderer.createImage(
        static_cast<uint32_t>(texWidth),
        static_cast<uint32_t>(texHeight),
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferDst |
        vk::ImageUsageFlagBits::eSampled,
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    vk::raii::CommandBuffer command = commandBuffer.beginSingleTimeCommands();

    transitionImageLayout(
        command,
        textureImage,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal
    );

    copyBufferToImage(
        command,
        stagingBuffer,
        textureImage,
        static_cast<uint32_t>(texWidth),
        static_cast<uint32_t>(texHeight)
    );

    transitionImageLayout(
        command,
        textureImage,
        vk::ImageLayout::eTransferDstOptimal,
        vk::ImageLayout::eShaderReadOnlyOptimal
    );

    commandBuffer.endSingleTimeCommands(std::move(command));
}

void Texture::createTextureImageView()
{
    vk::ImageViewCreateInfo viewInfo
    {
        .image = *textureImage,
        .viewType = vk::ImageViewType::e2D,
        .format = vk::Format::eR8G8B8A8Srgb,
        .subresourceRange = 
        {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    textureImageView = vk::raii::ImageView
    (
        renderer.GetDevice(),
        viewInfo
    );
}

void Texture::createTextureSampler()
{
    vk::PhysicalDeviceProperties properties =
        renderer.GetPhysicalDevice().getProperties();

    vk::SamplerCreateInfo samplerInfo
    {
        .magFilter = vk::Filter::eLinear,
        .minFilter = vk::Filter::eLinear,
        .addressModeU = vk::SamplerAddressMode::eRepeat,
        .addressModeV = vk::SamplerAddressMode::eRepeat,
        .addressModeW = vk::SamplerAddressMode::eRepeat,
        .anisotropyEnable = vk::True,
        .maxAnisotropy = properties.limits.maxSamplerAnisotropy,
        .compareEnable = vk::False,
        .compareOp = vk::CompareOp::eAlways
    };

    samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
    samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;

    textureSampler = vk::raii::Sampler
    (
        renderer.GetDevice(),
        samplerInfo
    );
}
