#pragma once

#include "wengine/core/IModule.h"

#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <unordered_set>

struct KeyEvent
{
    int key;    // GLFW_KEY_*
    int action; // GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT
    int mods;   // GLFW_MOD_*
};

struct MouseButtonEvent
{
    int button; // GLFW_MOUSE_BUTTON_*
    int action;
    int mods;
};

struct MouseMoveEvent
{
    double x;
    double y;
};

struct ScrollEvent
{
    double xOffset;
    double yOffset;
};

class InputModule : public IModule
{
public:
    void init(ResourceRegistry& resources) override;
    void update(double deltaTime)          override;
    void render()                          override {}
    void shutdown()                        override;

    std::vector<std::string> getDependencies() override { return {}; }

    bool isKeyDown(int key) const;
    bool wasKeyPressed(int key) const;
    bool wasKeyReleased(int key) const;

    bool isMouseButtonDown(int button) const;
    bool wasMouseButtonPressed(int button) const;
    bool wasMouseButtonReleased(int button) const;

    double mouseX() const { return m_mouseX; }
    double mouseY() const { return m_mouseY; }
    double mouseDeltaX() const { return m_mouseDeltaX; }
    double mouseDeltaY() const { return m_mouseDeltaY; }
    double scrollDeltaX() const { return m_scrollDeltaX; }
    double scrollDeltaY() const { return m_scrollDeltaY; }

    void pushKeyEvent(int key, int action, int mods);
    void pushMouseButtonEvent(int button, int action, int mods);
    void pushMouseMove(double x, double y);
    void pushScroll(double xOffset, double yOffset);

private:
    std::vector<KeyEvent>         m_keyBuffer;
    std::vector<MouseButtonEvent> m_mouseButtonBuffer;
    std::vector<MouseMoveEvent>   m_mouseMoveBuffer;
    std::vector<ScrollEvent>      m_scrollBuffer;

    std::unordered_set<int> m_keysDown;
    std::unordered_set<int> m_keysPressedThisTick;
    std::unordered_set<int> m_keysReleasedThisTick;

    std::unordered_set<int> m_mouseButtonsDown;
    std::unordered_set<int> m_mouseButtonsPressedThisTick;
    std::unordered_set<int> m_mouseButtonsReleasedThisTick;

    double m_mouseX = 0.0;
    double m_mouseY = 0.0;
    double m_mouseDeltaX = 0.0;
    double m_mouseDeltaY = 0.0;

    double m_scrollDeltaX = 0.0;
    double m_scrollDeltaY = 0.0;

    // accumulate these between ticks
    double m_pendingMouseDeltaX = 0.0;
    double m_pendingMouseDeltaY = 0.0;
    double m_pendingScrollDeltaX = 0.0;
    double m_pendingScrollDeltaY = 0.0;

    double m_lastMouseX = 0.0;
    double m_lastMouseY = 0.0;
    bool   m_firstMouse = true;
};
