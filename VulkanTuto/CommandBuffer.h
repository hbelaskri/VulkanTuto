#pragma once
#include "Renderer.h"
#include "SwapChain.h"
#include "Pipeline.h"
#include <vector>
#include "VertexBuffer.h"
#include "UniformBuffer.h"

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

class CommandBuffer
{
public:
	CommandBuffer(Renderer& renderer, SwapChain& swapChain, Pipeline& pipeline, VertexBuffer& vertexBuffer, UniformBuffer& uniformBuffer, uint32_t queueIndex);
	void recordCommandBuffer(uint32_t imageIndex);
	void drawFrame();
	vk::raii::CommandBuffer beginSingleTimeCommands();

	void endSingleTimeCommands
	(
		vk::raii::CommandBuffer&& commandBuffer
	);

private:
	Renderer& renderer;
	SwapChain& swapChain;
	uint32_t queueIndex;
	Pipeline& pipeline;
	VertexBuffer& vertexBuffer;
	UniformBuffer& uniformBuffer;

	vk::raii::CommandPool commandPool = nullptr;

	std::vector<vk::raii::CommandBuffer> commandBuffers;
	std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
	std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
	std::vector<vk::raii::Fence> inFlightFences;

	uint32_t frameIndex = 0;

	void createCommandPool();
	void createCommandBuffers();
	void createSyncObjects();

	void transition_image_layout
	(
		vk::Image image,
		vk::ImageLayout old_layout,
		vk::ImageLayout new_layout,
		vk::AccessFlags2 src_access_mask,
		vk::AccessFlags2 dst_access_mask,
		vk::PipelineStageFlags2 src_stage_mask,
		vk::PipelineStageFlags2 dst_stage_mask,
		vk::ImageAspectFlags aspect_mask
	);
};

