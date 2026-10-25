#include "SwapChain.h"
#include <limits>
#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <tuple>

SwapChain::SwapChain(Renderer& renderer, GLFWwindow* window)
    : renderer(renderer), window(window)
{
    glfwSetWindowUserPointer(window, this);
    glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_TRUE);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

    createSwapChain();
    createImageViews();
    createDepthResources();
   
}

vk::Image SwapChain::GetImage(uint32_t imageIndex) const
{
    return swapChainImages[imageIndex];
}

vk::ImageView SwapChain::GetImageView(uint32_t imageIndex) const
{
    return *swapChainImageViews[imageIndex];
}

vk::Image SwapChain::GetDepthImage() const
{
    return *depthImage;
}

vk::ImageView SwapChain::GetDepthImageView() const
{
    return *depthImageView;
}

vk::Extent2D SwapChain::GetExtent() const
{
    return swapChainExtent;
}

vk::Format SwapChain::GetDepthFormat() 
{
    return findDepthFormat();
}

vk::Format SwapChain::GetImageFormat() const
{
    return swapChainSurfaceFormat.format;
}

void SwapChain::createSwapChain()
{
    auto surfaceCapabilities =
        renderer.GetPhysicalDevice().getSurfaceCapabilitiesKHR(
            *renderer.GetSurface());
    swapChainExtent =
        chooseSwapExtent(surfaceCapabilities, window);

    std::vector<vk::SurfaceFormatKHR> availableFormats =
        renderer.GetPhysicalDevice().getSurfaceFormatsKHR(
            *renderer.GetSurface());
    swapChainSurfaceFormat =
        chooseSwapSurfaceFormat(availableFormats);

    std::vector<vk::PresentModeKHR> availablePresentModes =
        renderer.GetPhysicalDevice().getSurfacePresentModesKHR(
            *renderer.GetSurface());
    auto swapChainPresentMode =
        chooseSwapPresentMode(availablePresentModes);


    uint32_t minImageCount =
        chooseSwapMinImageCount(surfaceCapabilities);

    vk::SwapchainCreateInfoKHR swapChainCreateInfo
    {
        .surface = *renderer.GetSurface(),
        .minImageCount = minImageCount,
        .imageFormat = swapChainSurfaceFormat.format,
        .imageColorSpace = swapChainSurfaceFormat.colorSpace,
        .imageExtent = swapChainExtent,
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .preTransform = surfaceCapabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = swapChainPresentMode,
        .clipped = true
    };

    swapChain = vk::raii::SwapchainKHR(
        renderer.GetDevice(),
        swapChainCreateInfo);

    swapChainImages = swapChain.getImages();
}


vk::SurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(
    std::vector<vk::SurfaceFormatKHR> const& availableFormats)
{
    assert(!availableFormats.empty());

    const auto formatIt = std::ranges::find_if(
        availableFormats,
        [](const auto& format) 
        {
            return format.format == vk::Format::eB8G8R8A8Srgb &&
                format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
        });

    return formatIt != availableFormats.end()
        ? *formatIt
        : availableFormats[0];
}


vk::PresentModeKHR SwapChain::chooseSwapPresentMode(
    std::vector<vk::PresentModeKHR> const& availablePresentModes)
{
    assert(std::ranges::any_of(
        availablePresentModes,
        [](auto presentMode) 
        {
            return presentMode == vk::PresentModeKHR::eFifo;
        }));

    return std::ranges::any_of(
        availablePresentModes,
        [](const vk::PresentModeKHR value) 
        {
            return vk::PresentModeKHR::eMailbox == value;
        })
        ? vk::PresentModeKHR::eMailbox
        : vk::PresentModeKHR::eFifo;
}


vk::Extent2D SwapChain::chooseSwapExtent(
    vk::SurfaceCapabilitiesKHR const& capabilities,
    GLFWwindow* window)
{
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    return 
    {
        std::clamp<uint32_t>
        (
            width,
            capabilities.minImageExtent.width,
            capabilities.maxImageExtent.width
        ),

        std::clamp<uint32_t>
        (
            height,
            capabilities.minImageExtent.height,
            capabilities.maxImageExtent.height
        )
    };
}

uint32_t SwapChain::chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities)
{
    auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);

    if ((0 < surfaceCapabilities.maxImageCount) &&
        (surfaceCapabilities.maxImageCount < minImageCount))
    {
        minImageCount = surfaceCapabilities.maxImageCount;
    }

    return minImageCount;
}

void SwapChain::createImageViews()
{
    assert(swapChainImageViews.empty());

    vk::ImageViewCreateInfo imageViewCreateInfo
    {
        .viewType = vk::ImageViewType::e2D,
        .format = swapChainSurfaceFormat.format,
        .subresourceRange = 
        {
            vk::ImageAspectFlagBits::eColor,
            0, 1, 0, 1
        }

    };

    imageViewCreateInfo.components = 
    {
    vk::ComponentSwizzle::eIdentity,
    vk::ComponentSwizzle::eIdentity,
    vk::ComponentSwizzle::eIdentity,
    vk::ComponentSwizzle::eIdentity
    };
    
    for (auto& image : swapChainImages)
    {
        imageViewCreateInfo.image = image;
        swapChainImageViews.emplace_back(renderer.GetDevice(), imageViewCreateInfo);
    }

}

void SwapChain::cleanupSwapChain()
{
    depthImageView = nullptr;
    depthImage = nullptr;
    depthImageMemory = nullptr;

    swapChainImageViews.clear();
    swapChain = nullptr;
}

void SwapChain::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto swapChain = reinterpret_cast<SwapChain*>(glfwGetWindowUserPointer(window));
    swapChain->framebufferResized = true;
}


const vk::raii::SwapchainKHR& SwapChain::GetSwapChain() const
{
    return swapChain;
}

size_t SwapChain::GetImageCount() const
{
    return swapChainImages.size();
}

void SwapChain::recreateSwapChain()
{
    int width = 0, height = 0;

    glfwGetFramebufferSize(window, &width, &height);

    while ((width == 0 || height == 0) && !glfwWindowShouldClose(window))
    {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    if (glfwWindowShouldClose(window))
    {
        return;
    }

    renderer.GetDevice().waitIdle();

    cleanupSwapChain();

    createSwapChain();
    createImageViews();
    createDepthResources();

}

vk::Format SwapChain::findSupportedFormat
(
    const std::vector<vk::Format>& candidates,
    vk::ImageTiling tiling,
    vk::FormatFeatureFlags features)
{
    for (vk::Format format : candidates)
    {
        vk::FormatProperties props =
            renderer.GetPhysicalDevice().getFormatProperties(format);

        if (tiling == vk::ImageTiling::eLinear &&
            (props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == vk::ImageTiling::eOptimal &&
            (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

vk::Format SwapChain::findDepthFormat()
{
    return findSupportedFormat
    (
        {
            vk::Format::eD32Sfloat,
            vk::Format::eD32SfloatS8Uint,
            vk::Format::eD24UnormS8Uint
        },
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
}

void SwapChain::createDepthResources()
{
    vk::Format depthFormat = findDepthFormat();

    std::tie(depthImage, depthImageMemory) = renderer.createImage
    (
        swapChainExtent.width,
        swapChainExtent.height,
        depthFormat,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    vk::ImageViewCreateInfo viewInfo
    {
        .image = *depthImage,
        .viewType = vk::ImageViewType::e2D,
        .format = depthFormat,
        .subresourceRange =
        {
            .aspectMask = vk::ImageAspectFlagBits::eDepth,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    depthImageView = vk::raii::ImageView
    (
        renderer.GetDevice(),
        viewInfo
    );
}
