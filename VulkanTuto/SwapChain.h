#pragma once
#include "Renderer.h"
#include <vector>

class SwapChain
{
public:
    SwapChain(Renderer& renderer, GLFWwindow* window);
    vk::Format GetImageFormat() const;
    vk::Image GetImage(uint32_t imageIndex) const;
    vk::ImageView GetImageView(uint32_t imageIndex) const;
    vk::Image GetDepthImage() const;
    vk::ImageView GetDepthImageView() const;
    vk::Extent2D GetExtent() const;
    vk::Format GetDepthFormat();
    const vk::raii::SwapchainKHR& GetSwapChain() const;
    size_t GetImageCount() const;
    void recreateSwapChain();
    bool framebufferResized = false;

private:
    Renderer& renderer;
    GLFWwindow* window;
    vk::raii::SwapchainKHR swapChain = nullptr;
    std::vector<vk::Image> swapChainImages;

    void createSwapChain();
    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(
        std::vector<vk::SurfaceFormatKHR> const& availableFormats);
    vk::PresentModeKHR chooseSwapPresentMode(
        std::vector<vk::PresentModeKHR> const& availablePresentModes);
    vk::Extent2D chooseSwapExtent(
        vk::SurfaceCapabilitiesKHR const& capabilities,
        GLFWwindow* window);

    uint32_t chooseSwapMinImageCount(
        vk::SurfaceCapabilitiesKHR const& surfaceCapabilities);

    vk::SurfaceFormatKHR swapChainSurfaceFormat;
    vk::Extent2D swapChainExtent;

    std::vector<vk::raii::ImageView> swapChainImageViews;

    vk::Format findSupportedFormat
    (
        const std::vector<vk::Format>& candidates,
        vk::ImageTiling tiling,
        vk::FormatFeatureFlags features
    );

    vk::Format findDepthFormat();
    void createDepthResources();
    
    vk::raii::Image        depthImage = nullptr;
    vk::raii::DeviceMemory depthImageMemory = nullptr;
    vk::raii::ImageView    depthImageView = nullptr;

    void createImageViews();
    void cleanupSwapChain();

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

    
};

