#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include "Renderer.h"
#include "SwapChain.h"
#include "Debug.h"
#include <iostream>
#include <stdexcept>
#include <utility>

static_assert(sizeof(SwapChain) > 0);

Renderer::~Renderer() = default;


void Renderer::CreateInstance()
{
	constexpr vk::ApplicationInfo application_info
	{

		.pApplicationName = "Hello",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "GC Vulkan",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = vk::ApiVersion14
	};
	
	

	std::vector requiredExtensions = GetRequiredExtensions();


	auto extensionProperties = context.enumerateInstanceExtensionProperties();

	for (uint32_t i = 0; i < requiredExtensions.size(); i++)
	{
		auto compareExtensions = [glfwExtension = requiredExtensions[i]](auto const& extensionProperty) 
			{
				return strcmp(extensionProperty.extensionName, glfwExtension) == 0; 
			};

		if (std::ranges::none_of(extensionProperties, compareExtensions))
		{
			throw std::runtime_error("Required GLFW extension not supported : " + std::string(requiredExtensions[i]));
		}
	}

	std::vector<char const*> requiredLayers = GetRequiredLayers();

	auto layerProperties = context.enumerateInstanceLayerProperties();

	auto unsupportedLayerIt = std::ranges::find_if(requiredLayers, [&layerProperties](auto const& requiredLayer) 
		{ 
			return std::ranges::none_of(layerProperties, [requiredLayer](auto const& layerProperty)
				{
					return strcmp(layerProperty.layerName, requiredLayer) == 0;
				});
		});

	if (unsupportedLayerIt != requiredLayers.end())
	{
		throw std::runtime_error("Required Layer not supported :" + std::string(*unsupportedLayerIt));
	}

	vk::InstanceCreateInfo create_info
	{
		.pApplicationInfo = &application_info,
		.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
		.ppEnabledLayerNames = requiredLayers.data(),
		.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
		.ppEnabledExtensionNames = requiredExtensions.data()
	};

	instance = vk::raii::Instance(context, create_info);
}


void Renderer::InitVulkan(GLFWwindow* window)
{
	CreateInstance();
	SetupDebugCallBack();
	createSurface(window);
	pickPhysicalDevice();
	createLogicalDevice();

}

vk::raii::PhysicalDevice const& Renderer::GetPhysicalDevice() const
{
	return physicalDevice;
}

vk::raii::SurfaceKHR const& Renderer::GetSurface() const
{
	return surface;
}

vk::raii::Device const& Renderer::GetDevice() const
{
	return device;
}

const vk::raii::Queue& Renderer::GetGraphicsQueue() const
{
	return graphicsQueue;
}

uint32_t Renderer::GetQueueIndex() const
{
	return queueIndex;
}

std::vector<const char*> Renderer::GetRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<char const*> requiredExtensions = {vk::EXTDebugUtilsExtensionName};
	std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	extensions.append_range(requiredExtensions);
	return extensions;
}

std::vector<const char*> Renderer:: GetRequiredLayers()
{

	const std::vector<char const*> validationLayers = { "VK_LAYER_KHRONOS_validation" }; 



	std::vector<char const*> requiredLayers;
	if (Debug::enableDebug)
		requiredLayers.assign_range(validationLayers);
	return requiredLayers;
}

void Renderer::SetupDebugCallBack()
{
	vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
	vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

	vk::DebugUtilsMessengerCreateInfoEXT debugInfo{ .messageSeverity = severityFlags,.messageType = messageTypeFlags, .pfnUserCallback = &DebugCallback };

	debugMessenger = instance.createDebugUtilsMessengerEXT(debugInfo);
}

void Renderer::createLogicalDevice()
{
	std::vector<vk::QueueFamilyProperties>QueueFamilyProperties = physicalDevice.getQueueFamilyProperties();

	queueIndex = ~0;
	for (uint32_t qfpIndex = 0; qfpIndex < QueueFamilyProperties.size(); qfpIndex++)
	{
		if ((QueueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) &&
			physicalDevice.getSurfaceSupportKHR(qfpIndex, *surface))
		{
			queueIndex = qfpIndex;
			break;
		}
	}
	if (queueIndex == ~0)
	{
		throw std::runtime_error("could not find a queue for graphics and presents -----> Terminating");
	}

	float queuePriority = 0.5f;
	vk::DeviceQueueCreateInfo deviceQueueCreateInfo
	{
	.queueFamilyIndex = queueIndex,
	.queueCount = 1,
	.pQueuePriorities = &queuePriority
	};

	vk::StructureChain<vk::PhysicalDeviceFeatures2,
		vk::PhysicalDeviceVulkan11Features,
		vk::PhysicalDeviceVulkan13Features,
		vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>

		featureChain = 
	{
		{ .features = { .samplerAnisotropy = true } },
		{ .shaderDrawParameters = true },
		{ .synchronization2 = true, .dynamicRendering = true },
		{ .extendedDynamicState = true }
	};
	vk::DeviceCreateInfo deviceCreateInfo
	{
	.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
	.queueCreateInfoCount = 1,
	.pQueueCreateInfos = &deviceQueueCreateInfo,
	.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtension.size()),
	.ppEnabledExtensionNames = requiredDeviceExtension.data()
	};

	device = vk::raii::Device(physicalDevice, deviceCreateInfo);
	graphicsQueue = vk::raii::Queue(device, queueIndex, 0);
}

void Renderer::createSurface(GLFWwindow* window)
{
	VkSurfaceKHR         _surface;
	if (glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != 0)
	{
		throw std::runtime_error("failed to create window surface!");
	}
	surface = vk:: raii::SurfaceKHR(instance, _surface);
}



bool Renderer::isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice)
{

	bool supportsVulkan1_3 = physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;


	auto queueFamilies = physicalDevice.getQueueFamilyProperties();
	bool supportsGraphics = std::ranges::any_of(queueFamilies, [](auto const& qfp)
		{ 
			return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics); 
		});


	auto availableDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
	bool supportsAllRequiredExtensions =
		std::ranges::all_of(requiredDeviceExtension,
			[&availableDeviceExtensions](auto const& requiredDeviceExtension)
			{
				return std::ranges::any_of(availableDeviceExtensions,
					[requiredDeviceExtension](auto const& availableDeviceExtension)
					{ 
						return strcmp(availableDeviceExtension.extensionName, requiredDeviceExtension) == 0; 
					});
			});

	auto features = physicalDevice.template getFeatures2<vk::PhysicalDeviceFeatures2,
		vk::PhysicalDeviceVulkan11Features,
		vk::PhysicalDeviceVulkan13Features,
		vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
	bool supportsRequiredFeatures =
		features.template get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
		features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
		features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;


	return supportsVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions && supportsRequiredFeatures;
}

void Renderer::pickPhysicalDevice()
{
	std::vector<vk::raii::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();
	auto const devIter = std::ranges::find_if(physicalDevices, [&](auto const& physicalDevice) 
		{ 
			return isDeviceSuitable(physicalDevice); 
		});
	if (devIter == physicalDevices.end())
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}
	physicalDevice = *devIter;
}




VKAPI_ATTR vk::Bool32 VKAPI_CALL Renderer::DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*)
{
	std::cout << "Validation layer: Type [" << vk::to_string(type) << "] - Message : " << pCallbackData->pMessage << std::endl;
	return vk::False;
	

}
std::pair<vk::raii::Buffer, vk::raii::DeviceMemory>Renderer::createBuffer
(vk::DeviceSize size,
	vk::BufferUsageFlags usage,
	vk::MemoryPropertyFlags properties)
{
	vk::BufferCreateInfo   bufferInfo
	{ .size = size, .usage = usage, .sharingMode = vk::SharingMode::eExclusive };
	vk::raii::Buffer       buffer = vk::raii::Buffer(GetDevice(), bufferInfo);
	vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();
	vk::MemoryAllocateInfo allocInfo{ .allocationSize = memRequirements.size, .memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties) };
	vk::raii::DeviceMemory bufferMemory = vk::raii::DeviceMemory(GetDevice(), allocInfo);
	buffer.bindMemory(*bufferMemory, 0);
	return { std::move(buffer), std::move(bufferMemory) };
}

std::pair<vk::raii::Image, vk::raii::DeviceMemory>
Renderer::createImage(
	uint32_t width,
	uint32_t height,
	vk::Format format,
	vk::ImageTiling tiling,
	vk::ImageUsageFlags usage,
	vk::MemoryPropertyFlags properties)
{
	vk::ImageCreateInfo imageInfo
	{
		.imageType = vk::ImageType::e2D,
		.format = format,
		.extent = { width, height, 1 },
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = vk::SampleCountFlagBits::e1,
		.tiling = tiling,
		.usage = usage,
		.sharingMode = vk::SharingMode::eExclusive
	};

	vk::raii::Image image(GetDevice(), imageInfo);

	vk::MemoryRequirements memRequirements =
		image.getMemoryRequirements();

	vk::MemoryAllocateInfo allocInfo
	{
		.allocationSize = memRequirements.size,
		.memoryTypeIndex = findMemoryType
		(
			memRequirements.memoryTypeBits,
			properties
		)
	};

	vk::raii::DeviceMemory imageMemory
	(
		GetDevice(),
		allocInfo
	);

	image.bindMemory(*imageMemory, 0);

	return
	{
		std::move(image),
		std::move(imageMemory)
	};
}

uint32_t Renderer::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
{
	vk::PhysicalDeviceMemoryProperties memProperties = GetPhysicalDevice().getMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) &&
			(memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}