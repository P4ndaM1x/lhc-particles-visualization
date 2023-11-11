#pragma once

namespace utils {

struct Spherical {
    float distance, theta, phi;

    float getX() const
    {
        return distance * cos(phi) * cos(theta);
    }
    float getY() const
    {
        return distance * sin(phi);
    }
    float getZ() const
    {
        return distance * cos(phi) * sin(theta);
    }
};

static float degToRad(const float degrees)
{
    return degrees * (2 * std::numbers::pi) / 360.f;
}

static float radToDeg(const float radians)
{
    return radians * 360.f / (2 * std::numbers::pi);
}

}