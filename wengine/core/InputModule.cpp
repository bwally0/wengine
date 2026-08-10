#include "wengine/core/InputModule.h"
#include "wengine/core/ResourceRegistry.h"

#include <spdlog/spdlog.h>

void InputModule::init(ResourceRegistry& resources)
{
    spdlog::info("InputModule: initialized");
}

void InputModule::update(double deltaTime)
{
    // clear edge states
    m_keysPressedThisTick.clear();
    m_keysReleasedThisTick.clear();
    m_mouseButtonsPressedThisTick.clear();
    m_mouseButtonsReleasedThisTick.clear();

    // clear key buffer
    for (const auto& event : m_keyBuffer)
    {
        if (event.action == GLFW_PRESS)
        {
            m_keysDown.insert(event.key);
            m_keysPressedThisTick.insert(event.key);
        }
        else if (event.action == GLFW_RELEASE)
        {
            m_keysDown.erase(event.key);
            m_keysReleasedThisTick.insert(event.key);
        }
    }
    m_keyBuffer.clear();

    // clear mouse buffer
    for (const auto& event : m_mouseButtonBuffer)
    {
        if (event.action == GLFW_PRESS)
        {
            m_mouseButtonsDown.insert(event.button);
            m_mouseButtonsPressedThisTick.insert(event.button);
        }
        else if (event.action == GLFW_RELEASE)
        {
            m_mouseButtonsDown.erase(event.button);
            m_mouseButtonsReleasedThisTick.insert(event.button);
        }
    }
    m_mouseButtonBuffer.clear();

    // resolve mouse position
    if (!m_mouseMoveBuffer.empty())
    {
        auto& last = m_mouseMoveBuffer.back();
        m_mouseX = last.x;
        m_mouseY = last.y;
    }
    m_mouseMoveBuffer.clear();

    // consume deltas
    m_mouseDeltaX = m_pendingMouseDeltaX;
    m_mouseDeltaY = m_pendingMouseDeltaY;
    m_pendingMouseDeltaX = 0.0;
    m_pendingMouseDeltaY = 0.0;

    m_scrollDeltaX = m_pendingScrollDeltaX;
    m_scrollDeltaY = m_pendingScrollDeltaY;
    m_pendingScrollDeltaX = 0.0;
    m_pendingScrollDeltaY = 0.0;

    m_scrollBuffer.clear();
}

void InputModule::shutdown()
{
    spdlog::info("InputModule: shutdown");
}

bool InputModule::isKeyDown(int key) const
{
    return m_keysDown.count(key) > 0;
}

bool InputModule::wasKeyPressed(int key) const
{
    return m_keysPressedThisTick.count(key) > 0;
}

bool InputModule::wasKeyReleased(int key) const
{
    return m_keysReleasedThisTick.count(key) > 0;
}

bool InputModule::isMouseButtonDown(int button) const
{
    return m_mouseButtonsDown.count(button) > 0;
}

bool InputModule::wasMouseButtonPressed(int button) const
{
    return m_mouseButtonsPressedThisTick.count(button) > 0;
}

bool InputModule::wasMouseButtonReleased(int button) const
{
    return m_mouseButtonsReleasedThisTick.count(button) > 0;
}

void InputModule::pushKeyEvent(int key, int action, int mods)
{
    m_keyBuffer.push_back({ key, action, mods });
}

void InputModule::pushMouseButtonEvent(int button, int action, int mods)
{
    m_mouseButtonBuffer.push_back({ button, action, mods });
}

void InputModule::pushMouseMove(double x, double y)
{
    m_mouseMoveBuffer.push_back({ x, y });

    if (m_firstMouse)
    {
        m_lastMouseX = x;
        m_lastMouseY = y;
        m_firstMouse = false;
    }

    m_pendingMouseDeltaX += x - m_lastMouseX;
    m_pendingMouseDeltaY += y - m_lastMouseY;
    m_lastMouseX = x;
    m_lastMouseY = y;
}

void InputModule::pushScroll(double xOffset, double yOffset)
{
    m_scrollBuffer.push_back({ xOffset, yOffset });
    m_pendingScrollDeltaX += xOffset;
    m_pendingScrollDeltaY += yOffset;
}
