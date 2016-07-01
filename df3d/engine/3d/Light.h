#pragma once

namespace df3d {

// TODO: this class needs to be refactored.
class DF3D_DLL Light
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

    glm::vec4 m_diffuse;
    glm::vec4 m_specular;

    float m_constantAttenuation;
    float m_linearAttenuation;
    float m_quadraticAttenuation;

    glm::vec3 m_direction;     // Only for directional light.

    std::string m_name;

public:
    Light(Type type);
    ~Light();

    Type getType() const { return m_type; }

    void setDirection(const glm::vec3 &dir);
    void setDiffuseIntensity(const glm::vec3 &diffuse);
    void setDiffuseIntensity(float rd, float gd, float bd);
    void setSpecularIntensity(const glm::vec3 &specular);
    void setSpecularIntensity(float rs, float gs, float bs);
    void setName(const std::string &name) { m_name = name; }

    const glm::vec4& getDiffuseColor() const { return m_diffuse; }
    const glm::vec4& getSpecularColor() const { return m_specular; }
    glm::vec3 getDirection() const { return m_direction; }
    glm::vec3 getPosition() const;
    const std::string& getName() const { return m_name; }

    void setConstantAttenuation(float atten);
    void setLinearAttenuation(float atten);
    void setQuadraticAttenuation(float atten);

    float getConstantAttenuation() const { return m_constantAttenuation; }
    float getLinearAttenuation() const { return m_linearAttenuation; }
    float getQuadraticAttenuation() const { return m_quadraticAttenuation; }

};

}
