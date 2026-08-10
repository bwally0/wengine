#pragma once

struct Camera
{
    float fov       = 45.0f;
    float nearPlane = 0.1f;
    float farPlane  = 1000.0f;
    bool  active    = false;
    int   priority  = 0; // highest priority wins
};
