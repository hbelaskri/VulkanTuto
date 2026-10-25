#include "Pipeline.h"
#include <fstream>
#include <stdexcept>
#include "Vertex.h"
#include <array>

static std::vector<char> readFile(const std::string& filename) 
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) 
    {
        throw std::runtime_error("failed to open file!");
    }
    std::vector<char> buffer(file.tellg());

    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));

    return buffer;
}


Pipeline::Pipeline(Renderer& renderer, SwapChain& swapChain)
    : renderer(renderer), swapChain(swapChain)
{
    createDescriptorSetLayout();
    createGraphicsPipeline();
}

const vk::raii::Pipeline& Pipeline::GetGraphicsPipeline() const
{
    return graphicsPipeline;
}

const vk::raii::DescriptorSetLayout& Pipeline::GetDescriptorSetLayout() const
{
    return descriptorSetLayout;
}

void Pipeline::createDescriptorSetLayout()
{
    std::array<vk::DescriptorSetLayoutBinding, 2> bindings
    {
        {
            {
                .binding = 0,
                .descriptorType = vk::DescriptorType::eUniformBuffer,
                .descriptorCount = 1,
                .stageFlags = vk::ShaderStageFlagBits::eVertex
            },
            {
                .binding = 1,
                .descriptorType = vk::DescriptorType::eCombinedImageSampler,
                .descriptorCount = 1,
                .stageFlags = vk::ShaderStageFlagBits::eFragment
            }
        }
    };

    vk::DescriptorSetLayoutCreateInfo layoutInfo
    {
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindings = bindings.data()
    };

    descriptorSetLayout = vk::raii::DescriptorSetLayout
    (
        renderer.GetDevice(),
        layoutInfo
    );
}
void Pipeline::createGraphicsPipeline()
{
    vk::raii::ShaderModule shaderModule = createShaderModule(readFile("shaders/slang.spv"));

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo
    { 
        .stage = vk::ShaderStageFlagBits::eVertex,
        .module = shaderModule, 
        .pName = "vertMain" 
    };

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo
    { 
        .stage = vk::ShaderStageFlagBits::eFragment, 
        .module = shaderModule, 
        .pName = "fragMain" 
    };

    vk::PipelineShaderStageCreateInfo shaderStages[] = 
    { vertShaderStageInfo, fragShaderStageInfo };

    std::vector<vk::DynamicState> dynamicStates = 
    { vk::DynamicState::eViewport, vk::DynamicState::eScissor };

    vk::PipelineDynamicStateCreateInfo dynamicState
    { .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
      .pDynamicStates = dynamicStates.data() 
    };

    auto                                     bindingDescription = Vertex::getBindingDescription();
    auto                                     attributeDescriptions = Vertex::getAttributeDescriptions();
    vk::PipelineVertexInputStateCreateInfo   vertexInputInfo
    {               
         .vertexBindingDescriptionCount = 1,
         .pVertexBindingDescriptions = &bindingDescription,
         .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
         .pVertexAttributeDescriptions = attributeDescriptions.data() 
    };

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly
    { 
        .topology = vk::PrimitiveTopology::eTriangleList
    };
    vk::PipelineViewportStateCreateInfo viewportState
    { 
        .viewportCount = 1, 
        .scissorCount = 1 
    };

    vk::PipelineRasterizationStateCreateInfo rasterizer
    {
        .depthClampEnable = vk::False,
        .rasterizerDiscardEnable = vk::False,
        .polygonMode = vk::PolygonMode::eFill,
        .cullMode = vk::CullModeFlagBits::eBack,
        .frontFace = vk::FrontFace::eCounterClockwise,
        .depthBiasEnable = vk::False,
        .lineWidth = 1.0f
    };

    vk::PipelineMultisampleStateCreateInfo multisampling
    { 
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
        .sampleShadingEnable = vk::False
    };

    vk::PipelineDepthStencilStateCreateInfo depthStencil
    {
        .depthTestEnable = vk::True,
        .depthWriteEnable = vk::True,
        .depthCompareOp = vk::CompareOp::eLess,
        .depthBoundsTestEnable = vk::False,
        .stencilTestEnable = vk::False
    };

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{
    .blendEnable = vk::False,
    .colorWriteMask = 
          vk::ColorComponentFlagBits::eR | 
          vk::ColorComponentFlagBits::eG |
          vk::ColorComponentFlagBits::eB |
          vk::ColorComponentFlagBits::eA 
    };

    vk::PipelineColorBlendStateCreateInfo colorBlending
    {
          .logicOpEnable = vk::False, 
          .logicOp = vk::LogicOp::eCopy, 
          .attachmentCount = 1, 
          .pAttachments = &colorBlendAttachment 
    };

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo
    {
         .setLayoutCount = 1,
         .pSetLayouts = &*descriptorSetLayout,
         .pushConstantRangeCount = 0
    };

    pipelineLayout = vk::raii::PipelineLayout(renderer.GetDevice(), pipelineLayoutInfo);

    vk::Format colorFormat = swapChain.GetImageFormat();
    vk::Format depthFormat = swapChain.GetDepthFormat();

    vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain =
    {

       { 
             .stageCount = 2,
             .pStages = shaderStages,
             .pVertexInputState = &vertexInputInfo,
             .pInputAssemblyState = &inputAssembly,
             .pViewportState = &viewportState,
             .pRasterizationState = &rasterizer,
             .pMultisampleState = &multisampling,
             .pDepthStencilState = &depthStencil,
             .pColorBlendState = &colorBlending,
             .pDynamicState = &dynamicState,
             .layout = pipelineLayout,
             .renderPass = nullptr 
       },

         { 
             .colorAttachmentCount = 1, 
             .pColorAttachmentFormats = &colorFormat,
             .depthAttachmentFormat = depthFormat
         }
    };

    graphicsPipeline = vk::raii::Pipeline
    (renderer.GetDevice(), nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());
}


vk::raii::ShaderModule Pipeline::createShaderModule(
    const std::vector<char>& code) const
{
    vk::ShaderModuleCreateInfo createInfo{
        .codeSize = code.size() * sizeof(char),
        .pCode = reinterpret_cast<const uint32_t*>(code.data())
    };

    vk::raii::ShaderModule shaderModule{
        renderer.GetDevice(), createInfo
    };

    return shaderModule;
}

const vk::raii::PipelineLayout& Pipeline::GetPipelineLayout() const
{
    return pipelineLayout;
}