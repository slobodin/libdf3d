#include "LightComponent.h"

#include <utils/JsonUtils.h>
#include <render/RenderQueue.h>
#include <scene/Node.h>
#include <components/TransformComponent.h>

namespace df3d {

const size_t LightComponent::MAX_LIGHTS = 8;
size_t LightComponent::NumLights = 0;

void LightComponent::onDraw(RenderQueue *ops)
{

}

LightComponent::LightComponent(Type type)
    : NodeComponent(ComponentType::LIGHT)
{
    if (NumLights >= MAX_LIGHTS)
    {
        glog << "Light limit is reached" << logwarn;
        return;
    }

    if (type != Type::DIRECTIONAL)
    {
        // FIXME:
        // Support other light types!
        glog << "Can not create light component. Unsupported light type" << logwarn;
        return;
    }

    m_type = type;
    m_lightId = NumLights++;

    m_diffuse.r = m_diffuse.g = m_diffuse.b = 0.5f;
    m_specular.r = m_specular.g = m_specular.b = 1.0f;

    m_constantAttenuation = 1.0f;
    m_linearAttenuation = 1.0f;
    m_quadraticAttenuation = 1.0f;
}

LightComponent::~LightComponent()
{
    NumLights--;
}

void LightComponent::setDirection(const glm::vec3 &dir)
{
    if (m_type != Type::DIRECTIONAL)
    {
        glog << "Trying to set direction to not directional light" << logwarn;
        return;
    }

    m_direction = dir;
    glm::normalize(m_direction);
}

void LightComponent::setDiffuseIntensity(const glm::vec3 &diffuse)
{
    setDiffuseIntensity(diffuse.r, diffuse.g, diffuse.b);
}

void LightComponent::setDiffuseIntensity(float rd, float gd, float bd)
{
    m_diffuse.r = rd;
    m_diffuse.g = gd;
    m_diffuse.b = bd;
}

void LightComponent::setSpecularIntensity(const glm::vec3 &specular)
{
    setSpecularIntensity(specular.r, specular.g, specular.b);
}

void LightComponent::setSpecularIntensity(float rs, float gs, float bs)
{
    m_specular.r = rs;
    m_specular.g = gs;
    m_specular.b = bs;
}

glm::vec3 LightComponent::getPosition() const
{
    return m_holder->transform()->getPosition();
}

void LightComponent::setConstantAttenuation(float atten)
{
    m_constantAttenuation = atten;
}

void LightComponent::setLinearAttenuation(float atten)
{
    m_linearAttenuation = atten;
}

void LightComponent::setQuadraticAttenuation(float atten)
{
    m_quadraticAttenuation = atten;
}

shared_ptr<NodeComponent> LightComponent::clone() const
{
    assert(false);
    // TODO:
    return nullptr;
}

}
