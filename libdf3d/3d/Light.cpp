#include "Light.h"

namespace df3d {

Light::Light(Type type)
{
    if (type != Type::DIRECTIONAL)
    {
        // FIXME:
        // Support other light types!
        glog << "Can not create light component. Unsupported light type" << logwarn;
        throw std::runtime_error("Not implemented!");
    }

    m_type = type;

    m_diffuse.r = m_diffuse.g = m_diffuse.b = 1.0f;
    m_specular.r = m_specular.g = m_specular.b = 1.0f;

    m_constantAttenuation = 1.0f;
    m_linearAttenuation = 1.0f;
    m_quadraticAttenuation = 1.0f;
}

Light::~Light()
{

}

void Light::setDirection(const glm::vec3 &dir)
{
    if (m_type != Type::DIRECTIONAL)
    {
        glog << "Trying to set direction to not directional light" << logwarn;
        return;
    }

    m_direction = dir;
    glm::normalize(m_direction);
}

void Light::setDiffuseIntensity(const glm::vec3 &diffuse)
{
    setDiffuseIntensity(diffuse.r, diffuse.g, diffuse.b);
}

void Light::setDiffuseIntensity(float rd, float gd, float bd)
{
    m_diffuse.r = rd;
    m_diffuse.g = gd;
    m_diffuse.b = bd;
}

void Light::setSpecularIntensity(const glm::vec3 &specular)
{
    setSpecularIntensity(specular.r, specular.g, specular.b);
}

void Light::setSpecularIntensity(float rs, float gs, float bs)
{
    m_specular.r = rs;
    m_specular.g = gs;
    m_specular.b = bs;
}

glm::vec3 Light::getPosition() const
{
    return glm::vec3();
}

void Light::setConstantAttenuation(float atten)
{
    m_constantAttenuation = atten;
}

void Light::setLinearAttenuation(float atten)
{
    m_linearAttenuation = atten;
}

void Light::setQuadraticAttenuation(float atten)
{
    m_quadraticAttenuation = atten;
}

}
