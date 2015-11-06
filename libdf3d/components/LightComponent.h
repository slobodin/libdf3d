#pragma once

#include "NodeComponent.h"

namespace df3d {

class DF3D_DLL LightComponent : public NodeComponent
{
public:
    enum class Type
    {
        // Only directional light works (due to android...)
        // FIXME:
        // Rewrite shaders.
        DIRECTIONAL,
        POINT,
        SPOT
    };

private:
    Type m_type;

    static const size_t MAX_LIGHTS;
    static size_t NumLights;

    unsigned int m_lightId;

    bool m_isEnabled = true;

    glm::vec3 m_diffuse;
    glm::vec3 m_specular;

    float m_constantAttenuation;
    float m_linearAttenuation;
    float m_quadraticAttenuation;

    glm::vec3 m_direction;     // Only for directional light.

    void onDraw(RenderQueue *ops) override;

public:
    LightComponent(Type type);
    ~LightComponent();

    void turnon() { m_isEnabled = true; }
    void turnoff() { m_isEnabled = false; }

    bool enabled() const { return m_isEnabled; }
    Type type() const { return m_type; }
    unsigned int id() const { return m_lightId; }

    void setDirection(const glm::vec3 &dir);
    void setDiffuseIntensity(const glm::vec3 &diffuse);
    void setDiffuseIntensity(float rd, float gd, float bd);
    void setSpecularIntensity(const glm::vec3 &specular);
    void setSpecularIntensity(float rs, float gs, float bs);

    const glm::vec3 &getDiffuseColor() const { return m_diffuse; }
    const glm::vec3 &getSpecularColor() const { return m_specular; }
    glm::vec3 getDirection() const { return m_direction; }
    glm::vec3 getPosition() const;

    void setConstantAttenuation(float atten);
    void setLinearAttenuation(float atten);
    void setQuadraticAttenuation(float atten);

    float getConstantAttenuation() const { return m_constantAttenuation; }
    float getLinearAttenuation() const { return m_linearAttenuation; }
    float getQuadraticAttenuation() const { return m_quadraticAttenuation; }

    shared_ptr<NodeComponent> clone() const override;
};

}
