#include "Application.h"

int main()
{
   
    Application app(vertices, indices, "../Textures/texture.jpg");
    app.Init();
    app.Run();

    return 0;
}
