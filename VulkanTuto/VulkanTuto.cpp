#include "Application.h"

int main()
{
   
    Application app(vertices, indices, "textures/texture.jpg");
    app.Init();
    app.Run();

    return 0;
}
