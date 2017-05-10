#pragma once

namespace df3d {

class Light
{
public:
    enum class Type
    {
        // Only directional light works (due to android...)
        // FIXME:
        // Rewrite shaders.
        DIRECTIONAL,
        OMNI,
        SPOT
    };

private:
    Type m_type;
    glm::vec3 m_direction;     // Only for directional light.
    glm::vec3 m_color = glm::vec3(1.0f, 1.0f, 1.0f);
    float m_intensity = 0.0f;

public:
    Light();
    ~Light() = default;

    Type getType() const { return m_type; }

    void setDirection(const glm::vec3 &dir) { m_direction = glm::normalize(dir); }
    void setColor(const glm::vec3 &color) { m_color = color; }
    void setIntensity(float intensity) { m_intensity = intensity; }

    const glm::vec3& getDirection() const { return m_direction; }
    const glm::vec3& getColor() const { return m_color; }
    float getIntensity() const { return m_intensity; }
};

}
