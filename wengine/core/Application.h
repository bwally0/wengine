#pragma once

#include "wengine/core/ModuleRegistry.h"
#include "wengine/core/ResourceRegistry.h"
#include "wengine/core/EventBus.h"
#include "wengine/core/Window.h"

#include <string>
#include <memory>

struct GLFWwindow; // forward declare
class AssetRegistry;

struct AppConfig
{
    int         width     = 800;
    int         height    = 600;
    std::string title     = "wengine";
    double      timestep  = 1.0 / 60.0; // 60Hz
};

// core engine modules
class InputModule;
class SceneModule;
class RenderModule;

class Application
{
public:
    explicit Application(AppConfig config = {});
    ~Application();

    void registerModule(std::string name, std::unique_ptr<IModule> module);

    void run();

    InputModule&  getInputModule()  { return *m_inputModule; }
    SceneModule&  getSceneModule()  { return *m_sceneModule; }
    RenderModule& getRenderModule() { return *m_renderModule; }

private:
    bool initGLFW();
    bool initGLAD();
    void initCallbacks();
    void initCoreModules();
    void shutdown();

    GLFWwindow*      m_window = nullptr;
    
    Window           m_windowResource;
    AppConfig        m_config;
    EventBus         m_eventBus;

    ResourceRegistry m_resourceRegistry;
    ModuleRegistry   m_moduleRegistry;

    InputModule*  m_inputModule  = nullptr;
    SceneModule*  m_sceneModule  = nullptr;
    RenderModule* m_renderModule = nullptr;

    // Asset management
    std::unique_ptr<AssetRegistry> m_assetRegistry;
};
