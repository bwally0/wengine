#include "wengine/core/Application.h"
#include "wengine/core/events/WindowResizeEvent.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

Application::Application(AppConfig config)
    : m_config(std::move(config))
{
}

Application::~Application()
{
    shutdown();
}

void Application::registerModule(std::string name, std::unique_ptr<IModule> module)
{
    m_moduleRegistry.registerModule(std::move(name), std::move(module));
}

void Application::run()
{
    if (!initGLFW()) return;
    if (!initGLAD())
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        return;
    }

    initCallbacks();

    // register engine services before any module inits
    m_serviceLocator.provide<EventBus>(&m_eventBus);

    spdlog::info("OpenGL version: {}", (const char*)glGetString(GL_VERSION));

    m_moduleRegistry.init();

    // fixed timestep loop
    double lastTime    = glfwGetTime();
    double accumulator = 0.0;

    while (!glfwWindowShouldClose(m_window))
    {
        double currentTime = glfwGetTime();
        double frameTime   = currentTime - lastTime;
        lastTime           = currentTime;

        // cap to prevent stall on slow frames
        if (frameTime > 0.25) frameTime = 0.25;

        accumulator += frameTime;

        while (accumulator >= m_config.timestep)
        {
            m_moduleRegistry.update(m_config.timestep); // updates are fixed time
            accumulator -= m_config.timestep;
        }

        glClear(GL_COLOR_BUFFER_BIT);
        m_moduleRegistry.render();                      // renders are variable time

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    m_moduleRegistry.shutdown();
}

bool Application::initGLFW()
{
    glfwSetErrorCallback([](int error, const char* desc) {
        spdlog::error("GLFW error {}: {}", error, desc);
    });

    if (!glfwInit())
    {
        spdlog::error("Failed to initialize GLFW");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(m_config.width, m_config.height, m_config.title.c_str(), nullptr, nullptr);

    if (!m_window)
    {
        spdlog::error("Failed to create GLFW window");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // vsync

    return true;
}

bool Application::initGLAD()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        spdlog::error("Failed to initialize GLAD");
        return false;
    }
    return true;
}

void Application::initCallbacks()
{
    glfwSetWindowUserPointer(m_window, this);

    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int w, int h) {
        glViewport(0, 0, w, h);
        auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        app->m_eventBus.publish(WindowResizeEvent{ w, h });
    });

    glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    });
}

void Application::shutdown()
{
    if (m_window)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}
