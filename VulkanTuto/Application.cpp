#include "Application.h"
#include <glm/gtc/matrix_transform.hpp>

Application::Application(const std::vector<Vertex>& vertices,
    const std::vector<uint16_t>& indices, const std::string& texturePath)
    : vertices(vertices), indices(indices), texturePath(texturePath)
{
}

Application::~Application()
{
    Destroy();
}

void Application::Init()
{
    InitWindow();
    InitVulkan();
}


void Application::Run()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        float time = static_cast<float>(glfwGetTime());

        glm::mat4 model = glm::rotate
        (
            glm::mat4(1.0f),
            time * glm::radians(90.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)
        );

        uniformBuffer->setModelMatrix(model);

        commandBuffer->drawFrame();
    }

    renderer.GetDevice().waitIdle();
}
void Application::Destroy()
{
    delete texture;
    texture = nullptr;

    delete commandBuffer;
    commandBuffer = nullptr;

    delete uniformBuffer;
    uniformBuffer = nullptr;

    delete vertexBuffer;
	vertexBuffer = nullptr;

    delete pipeline;
    pipeline = nullptr;

    delete swapChain;
    swapChain = nullptr;

    
    DestroyWindow();
}
void Application::InitWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, "hello", nullptr, nullptr);
}
void Application::InitVulkan()
{
    renderer.InitVulkan(window);

    swapChain = new SwapChain(renderer, window);
    pipeline = new Pipeline(renderer, *swapChain);
    vertexBuffer = new VertexBuffer(renderer, vertices, indices);
    uniformBuffer = new UniformBuffer(renderer, *swapChain, *pipeline);
    commandBuffer = new CommandBuffer(renderer, *swapChain, *pipeline, *vertexBuffer, *uniformBuffer, renderer.GetQueueIndex());
    texture = new Texture(renderer, *commandBuffer, texturePath);
    uniformBuffer->createDescriptorSets(*texture);

}

void Application::DestroyWindow()
{
    if (window)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    glfwTerminate();
}