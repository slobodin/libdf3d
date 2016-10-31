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

    std::string m_name;
    glm::vec3 m_direction;     // Only for directional light.
    glm::vec3 m_color = glm::vec3(1.0f, 1.0f, 1.0f);
    float m_intensity = 1.0f;

public:
    Light(Type type);
    ~Light() = default;

    Type getType() const { return m_type; }

    void setDirection(const glm::vec3 &dir) { m_direction = glm::normalize(dir); }
    void setColor(const glm::vec3 &color) { m_color = color; }
    void setIntensity(float intensity) { m_intensity = intensity; }
    void setName(const std::string &name) { m_name = name; }

    const glm::vec3& getDirection() const { return m_direction; }
    const glm::vec3& getColor() const { return m_color; }
    float getIntensity() const { return m_intensity; }
    const std::string& getName() const { return m_name; }
};

}
