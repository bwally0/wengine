#include "Application.h"

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
    m_registry.registerModule(std::move(name), std::move(module));
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
    m_locator.provide<EventBus>(&m_eventBus);

    spdlog::info("OpenGL version: {}", (const char*)glGetString(GL_VERSION));

    m_registry.init();

    // fixed timestep loop
    double lastTime = glfwGetTime();
    double accumulatedTime = 0.0;

    while (!glfwWindowShouldClose(m_window))
    {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        if (deltaTime > 0.25) deltaTime = 0.25;

        accumulatedTime += deltaTime;

        while (accumulatedTime >= m_config.fixedTimestep)
        {
            m_registry.update(m_config.fixedTimestep);
            accumulatedTime -= m_config.fixedTimestep;
        }

        glClear(GL_COLOR_BUFFER_BIT);
        m_registry.render();

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    m_registry.shutdown();
}

bool Application::initGLFW()
{
    glfwSetErrorCallback([](int error, const char* desc) {
        spdlog::error("GLFW error {}: {}", error, desc);
    });

    if (!glfwInit())
    {
        spdlog::error("failed to initialize GLFW");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(m_config.windowWidth, m_config.windowHeight, m_config.windowTitle.c_str(), nullptr, nullptr);

    if (!m_window)
    {
        spdlog::error("failed to create GLFW window");
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
        spdlog::error("failed to initialize GLAD");
        return false;
    }
    return true;
}

void Application::initCallbacks()
{
    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow*, int w, int h) {
        glViewport(0, 0, w, h);
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
