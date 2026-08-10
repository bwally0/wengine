#pragma once

#include "wengine/core/ModuleRegistry.h"
#include "wengine/core/ResourceRegistry.h"
#include "wengine/core/EventBus.h"

#include <string>

// forward declare to avoid pulling GLFW into every file that includes Application.h
struct GLFWwindow;

struct AppConfig
{
    int         width     = 800;
    int         height    = 600;
    std::string title     = "wengine";
    double      timestep  = 1.0 / 60.0; // 60Hz
};

class Application
{
public:
    explicit Application(AppConfig config = {});
    ~Application();

    void registerModule(std::string name, std::unique_ptr<IModule> module);

    void run();

    GLFWwindow* getWindow() { return m_window; }

private:
    bool initGLFW();
    bool initGLAD();
    void initCallbacks();
    void shutdown();

    AppConfig        m_config;
    GLFWwindow*      m_window = nullptr;
    EventBus         m_eventBus;
    
    ResourceRegistry m_resourceRegistry;
    ModuleRegistry   m_moduleRegistry;
};
