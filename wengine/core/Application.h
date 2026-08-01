#pragma once

#include "ModuleRegistry.h"
#include "ServiceLocator.h"
#include "EventBus.h"

#include <string>

// forward declare to avoid pulling GLFW into every file that includes Application.h
struct GLFWwindow;

struct AppConfig
{
    int windowWidth  = 800;
    int windowHeight = 600;
    std::string windowTitle  = "wengine";
    double fixedTimestep = 1.0 / 60.0; // 60Hz
};

class Application
{
public:
    explicit Application(AppConfig config = {});
    ~Application();

    void registerModule(std::string name, std::unique_ptr<IModule> module);

    void run();

    ServiceLocator& getServiceLocator() { return m_locator; }
    EventBus& getEventBus() { return m_eventBus; }

private:
    bool initGLFW();
    bool initGLAD();
    void initCallbacks();
    void shutdown();

    AppConfig m_config;
    GLFWwindow* m_window  = nullptr;

    EventBus m_eventBus;
    ServiceLocator m_locator;
    ModuleRegistry m_registry;
};
