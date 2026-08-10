#include "wengine/core/Application.h"
#include "GameModule.h"

int main()
{
    AppConfig config{
        .width  = 800,
        .height = 600,
        .title  = "wengine"
    };

    Application app(config);

    auto game = std::make_unique<GameModule>(
        app.getSceneModule(), app.getRenderModule(), app.getInputModule());
    app.registerModule("GameModule", std::move(game));

    app.run();

    return 0;
}
