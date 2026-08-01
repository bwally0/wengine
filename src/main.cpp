#include "wengine/core/Application.h"

int main()
{
    Application app({
        .width  = 800,
        .height = 600,
        .title  = "wengine"
    });

    app.run();

    return 0;
}
