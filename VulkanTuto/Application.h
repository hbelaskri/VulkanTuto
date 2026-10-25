#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "VertexBuffer.h"
#include "Renderer.h"
#include "SwapChain.h"

#include "Pipeline.h"
#include "CommandBuffer.h"
#include "UniformBuffer.h"
#include "Texture.h"
#include <vector>
#include <cstdint>
#include <string>

class Application
{
public :
	Application(const std::vector<Vertex>& vertices,
		const std::vector<uint16_t>& indices, const std::string& texturePath);
	~Application();

	int width = 800;
	int height = 600;
	GLFWwindow* window = nullptr;
	SwapChain* swapChain = nullptr;
	Pipeline* pipeline = nullptr;
	CommandBuffer* commandBuffer = nullptr;
	VertexBuffer* vertexBuffer = nullptr;
	UniformBuffer* uniformBuffer = nullptr;
	Texture* texture = nullptr;

	void Init();
	void Run();
	void Destroy();
private:

	std::string texturePath;
	const std::vector<Vertex>& vertices;
	const std::vector<uint16_t>& indices;

	void InitWindow();
	void InitVulkan();
	void DestroyWindow();
	Renderer renderer;
};
