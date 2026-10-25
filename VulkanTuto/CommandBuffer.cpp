#include "CommandBuffer.h"
#include <cassert>

CommandBuffer::CommandBuffer(Renderer& renderer, SwapChain& swapChain, Pipeline& pipeline, VertexBuffer& vertexBuffer, UniformBuffer& uniformBuffer, uint32_t queueIndex)
	: renderer(renderer), swapChain(swapChain), pipeline(pipeline), queueIndex(queueIndex), vertexBuffer(vertexBuffer), uniformBuffer(uniformBuffer)
{
	createCommandPool();
	createCommandBuffers();
    createSyncObjects();
}

void CommandBuffer::recordCommandBuffer(uint32_t imageIndex)
{
    auto& commandBuffer = commandBuffers[frameIndex];

    commandBuffer.begin({}); 
    transition_image_layout
    (
        swapChain.GetImage(imageIndex),
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal,
        {},
        vk::AccessFlagBits2::eColorAttachmentWrite,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::ImageAspectFlagBits::eColor
    );

    transition_image_layout
    (
        swapChain.GetDepthImage(),
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eDepthAttachmentOptimal,
        vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
        vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
        vk::PipelineStageFlagBits2::eEarlyFragmentTests |
        vk::PipelineStageFlagBits2::eLateFragmentTests,
        vk::PipelineStageFlagBits2::eEarlyFragmentTests |
        vk::PipelineStageFlagBits2::eLateFragmentTests,
        vk::ImageAspectFlagBits::eDepth
    );

    vk::ClearValue clearColor =
        vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);

    vk::RenderingAttachmentInfo attachmentInfo = 
    {
        .imageView = swapChain.GetImageView(imageIndex),
        .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = clearColor
    };

    vk::RenderingAttachmentInfo depthAttachment = {
    .imageView = swapChain.GetDepthImageView(),
    .imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
    .loadOp = vk::AttachmentLoadOp::eClear,
    .storeOp = vk::AttachmentStoreOp::eDontCare,
    .clearValue = vk::ClearDepthStencilValue(1.0f, 0)
    };

    vk::RenderingInfo renderingInfo = 
    {
        .renderArea = {
            .offset = { 0, 0 },
            .extent = swapChain.GetExtent()
        },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachmentInfo,
        .pDepthAttachment = &depthAttachment
    };

    commandBuffer.beginRendering(renderingInfo);

    commandBuffer.bindPipeline
    (
        vk::PipelineBindPoint::eGraphics,
        *pipeline.GetGraphicsPipeline()
    );
    commandBuffer.bindVertexBuffers(0, *vertexBuffer.GetVertexBuffer(), { 0 });
    commandBuffer.bindIndexBuffer(*vertexBuffer.GetIndexBuffer(), 0, vk::IndexType::eUint16);

    vk::Extent2D swapChainExtent = swapChain.GetExtent();

    commandBuffer.setViewport
    (0, vk::Viewport(0.0f,
        static_cast<float>(swapChainExtent.height), 
        static_cast<float>(swapChainExtent.width),
        -static_cast<float>(swapChainExtent.height), 
        0.0f, 1.0f)
    );

    commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapChainExtent
    ));

    commandBuffer.bindDescriptorSets
    (vk::PipelineBindPoint::eGraphics, 
        *pipeline.GetPipelineLayout(),
        0, 
        *uniformBuffer.GetDescriptorSet(frameIndex),
        nullptr);

    commandBuffer.drawIndexed(vertexBuffer.GetIndexCount(), 1, 0, 0, 0);

    commandBuffer.endRendering();

    transition_image_layout
    (
        swapChain.GetImage(imageIndex),
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        vk::AccessFlagBits2::eColorAttachmentWrite,
        {},
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eBottomOfPipe,
        vk::ImageAspectFlagBits::eColor
    );

    commandBuffer.end();
}

void CommandBuffer::transition_image_layout
(
    vk::Image image,
    vk::ImageLayout old_layout,
    vk::ImageLayout new_layout,
    vk::AccessFlags2 src_access_mask,
    vk::AccessFlags2 dst_access_mask,
    vk::PipelineStageFlags2 src_stage_mask,
    vk::PipelineStageFlags2 dst_stage_mask,
    vk::ImageAspectFlags aspect_mask)
{
	vk::ImageMemoryBarrier2 barrier = {
		.srcStageMask = src_stage_mask,
		.srcAccessMask = src_access_mask,
		.dstStageMask = dst_stage_mask,
		.dstAccessMask = dst_access_mask,
		.oldLayout = old_layout,
		.newLayout = new_layout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
		.subresourceRange = {
              .aspectMask = aspect_mask,
			   .baseMipLevel = 0,
			   .levelCount = 1,
			   .baseArrayLayer = 0,
			   .layerCount = 1 } };
	vk::DependencyInfo dependency_info = {
		.dependencyFlags = {},
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &barrier };
    commandBuffers[frameIndex].pipelineBarrier2(dependency_info);
}


void CommandBuffer::createCommandPool()
{
	vk::CommandPoolCreateInfo poolInfo
	{ .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
	  .queueFamilyIndex = queueIndex 
	};

	commandPool = vk::raii::CommandPool(renderer.GetDevice(), poolInfo);
}

void CommandBuffer::createCommandBuffers() 
{
    vk::CommandBufferAllocateInfo allocInfo
    { 
        .commandPool = commandPool, 
        .level = vk::CommandBufferLevel::ePrimary, 
        .commandBufferCount = MAX_FRAMES_IN_FLIGHT 
    };
    commandBuffers = vk::raii::CommandBuffers(renderer.GetDevice(), allocInfo);
}

vk::raii::CommandBuffer CommandBuffer::beginSingleTimeCommands()
{
    vk::CommandBufferAllocateInfo allocInfo
    {
        .commandPool = commandPool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1
    };

    vk::raii::CommandBuffer commandBuffer =
        std::move
        (
            vk::raii::CommandBuffers
            (
                renderer.GetDevice(),
                allocInfo
            ).front()
        );

    vk::CommandBufferBeginInfo beginInfo
    {
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit
    };

    commandBuffer.begin(beginInfo);

    return commandBuffer;
}

void CommandBuffer::endSingleTimeCommands
(vk::raii::CommandBuffer&& commandBuffer)
{
    commandBuffer.end();

    const vk::CommandBuffer commandBufferHandle = *commandBuffer;

    vk::SubmitInfo submitInfo
    {
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBufferHandle
    };

    renderer.GetGraphicsQueue().submit(submitInfo, nullptr);
    renderer.GetGraphicsQueue().waitIdle();
}

void CommandBuffer::drawFrame()
{
    auto fenceResult = renderer.GetDevice().waitForFences
    (
        *inFlightFences[frameIndex],
        vk::True,
        UINT64_MAX
    );

    if (fenceResult != vk::Result::eSuccess)
    {
        throw std::runtime_error("failed to wait for fence!");
    }

    auto [result, imageIndex] =
        swapChain.GetSwapChain().acquireNextImage
        (
            UINT64_MAX,
            *presentCompleteSemaphores[frameIndex],
            nullptr
        );

    if (result == vk::Result::eErrorOutOfDateKHR)
    {
        swapChain.recreateSwapChain();
        return;
    }

    if (result != vk::Result::eSuccess &&
        result != vk::Result::eSuboptimalKHR)
    {
        assert(result == vk::Result::eTimeout ||
            result == vk::Result::eNotReady);

        throw std::runtime_error("failed to acquire swap chain image!");
    }

    renderer.GetDevice().resetFences(*inFlightFences[frameIndex]);

    commandBuffers[frameIndex].reset();
    recordCommandBuffer(imageIndex);

    vk::PipelineStageFlags waitDestinationStageMask
    (
        vk::PipelineStageFlagBits::eColorAttachmentOutput
    );

    const vk::CommandBuffer commandBufferHandle =
        *commandBuffers[frameIndex];

    const vk::SubmitInfo submitInfo
    {
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*presentCompleteSemaphores[frameIndex],
        .pWaitDstStageMask = &waitDestinationStageMask,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBufferHandle,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &*renderFinishedSemaphores[imageIndex]
    };
    uniformBuffer.updateUniformBuffer(frameIndex);

    renderer.GetGraphicsQueue().submit
    (
        submitInfo,
        *inFlightFences[frameIndex]
    );

    const vk::SwapchainKHR swapchainHandle =
        *swapChain.GetSwapChain();

    const vk::PresentInfoKHR presentInfoKHR
    {
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*renderFinishedSemaphores[imageIndex],
        .swapchainCount = 1,
        .pSwapchains = &swapchainHandle,
        .pImageIndices = &imageIndex
    };

    result = renderer.GetGraphicsQueue().presentKHR(presentInfoKHR);

    if (result == vk::Result::eSuboptimalKHR ||
        result == vk::Result::eErrorOutOfDateKHR ||
        swapChain.framebufferResized)
    {
        swapChain.framebufferResized = false;
        swapChain.recreateSwapChain();
    }
    else
    {
        assert(result == vk::Result::eSuccess);
    }

    frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

void CommandBuffer::createSyncObjects()
{
    assert(presentCompleteSemaphores.empty() &&
        renderFinishedSemaphores.empty() &&
        inFlightFences.empty());

    for (size_t i = 0; i < swapChain.GetImageCount(); i++)
    {
        renderFinishedSemaphores.emplace_back
        (
            renderer.GetDevice(),
            vk::SemaphoreCreateInfo()
        );
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        presentCompleteSemaphores.emplace_back
        (
            renderer.GetDevice(),
            vk::SemaphoreCreateInfo()
        );

        inFlightFences.emplace_back
        (
            renderer.GetDevice(),
            vk::FenceCreateInfo
            {
                .flags = vk::FenceCreateFlagBits::eSignaled
            }
        );
    }
}