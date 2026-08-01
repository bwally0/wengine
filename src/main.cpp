#include "wengine/core/Application.h"

int main()
{
    Application app({ 
        .windowWidth = 800, 
        .windowHeight = 600, 
        .windowTitle = "wengine" 
    });

    app.run();

    return 0;
}
