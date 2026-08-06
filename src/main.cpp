#include "wengine/core/Application.h"
#include "wengine/scene/SceneModule.h"
#include "wengine/render/RenderModule.h"
#include "GameModule.h"

int main()
{
    AppConfig config{
        .width  = 800,
        .height = 600,
        .title  = "wengine"
    };

    Application app(config);

    auto scene    = std::make_unique<SceneModule>();
    auto renderer = std::make_unique<RenderModule>(*scene, app.getEventBus(), config.width, config.height);
    auto game     = std::make_unique<GameModule>(*scene);

    app.registerModule("SceneModule",  std::move(scene));
    app.registerModule("RenderModule", std::move(renderer));
    app.registerModule("GameModule",   std::move(game));

    app.run();

    return 0;
}
