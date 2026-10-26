#include "Application.h"
#include <iostream>
#include <exception>
#include <cstdlib>

const std::vector<Vertex> triangleVertices =
{
    { { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },
    { { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
    { { 0.0f,  0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.5f, 1.0f } }
};

const std::vector<uint16_t> triangleIndices =
{
    0, 1, 2
};

int main()
{
    try
    {
        Application app(triangleVertices, triangleIndices, "../Textures/triangle.jpg");

        app.Init();
        app.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Erreur : " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}