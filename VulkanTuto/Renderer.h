#pragma once
#define VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include "vulkan/vulkan_raii.hpp"
#include <vector>
#include <utility>


class Renderer
{
public:
    void CreateInstance();
    void InitVulkan(GLFWwindow* window);
    vk::raii::PhysicalDevice const& GetPhysicalDevice() const;
    vk::raii::SurfaceKHR const& GetSurface() const;
    vk::raii::Device const& GetDevice() const;
    ~Renderer();
    const vk::raii::Queue& GetGraphicsQueue() const;
    uint32_t GetQueueIndex() const;

    std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> createBuffer
    (vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);

    std::pair<vk::raii::Image, vk::raii::DeviceMemory> createImage
    (
        uint32_t width,
        uint32_t height,
        vk::Format format,
        vk::ImageTiling tiling,
        vk::ImageUsageFlags usage,
        vk::MemoryPropertyFlags properties
    );

    uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
    

private:
    vk::raii::Context context;
    vk::raii::Instance instance = nullptr;
    vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
    vk::raii::PhysicalDevice physicalDevice = nullptr;
    vk::raii::Device device = nullptr;
    vk::PhysicalDeviceFeatures deviceFeatures;
    vk::raii::Queue graphicsQueue = nullptr;
    vk::raii::SurfaceKHR surface = nullptr;
   
    std::vector<const char*> GetRequiredExtensions();
    std::vector<const char*> GetRequiredLayers();
    std::vector<const char*> requiredDeviceExtension = { vk::KHRSwapchainExtensionName };

    void SetupDebugCallBack();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSurface(GLFWwindow* window);

    bool isDeviceSuitable(
        vk::raii::PhysicalDevice const& physicalDevice);
  

    static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT, vk::DebugUtilsMessageTypeFlagsEXT, const vk::DebugUtilsMessengerCallbackDataEXT*, void*);
    uint32_t queueIndex = 0;
   
};