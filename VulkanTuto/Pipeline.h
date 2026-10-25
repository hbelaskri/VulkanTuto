#pragma once
#include "Renderer.h"
#include <vector>
#include "SwapChain.h"

class Pipeline
{
public:
	Pipeline(Renderer& renderer, SwapChain& swapChain);
	const vk::raii::Pipeline& GetGraphicsPipeline() const;
	const vk::raii::DescriptorSetLayout& GetDescriptorSetLayout() const;
	const vk::raii::PipelineLayout& GetPipelineLayout() const;


private: 
	Renderer& renderer;
	SwapChain& swapChain;
	void createDescriptorSetLayout();
	void createGraphicsPipeline();
	[[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code) const;
	
	vk::raii::DescriptorSetLayout descriptorSetLayout = nullptr;
	vk::raii::PipelineLayout pipelineLayout = nullptr;
	vk::raii::Pipeline graphicsPipeline = nullptr;
};

