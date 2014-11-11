#include "df3d_pch.h"
#include "LightComponent.h"

#include <utils/JsonHelpers.h>
#include <render/RenderQueue.h>
#include <scene/Node.h>
#include <components/TransformComponent.h>

namespace df3d { namespace components {

const size_t LightComponent::MAX_LIGHTS = 8;
size_t LightComponent::NumLights = 0;

void LightComponent::onDraw(render::RenderQueue *ops)
{
    if (enabled())
        ops->lights.push_back(this);
}

LightComponent::LightComponent(Type type)
    : NodeComponent(ComponentType::LIGHT)
{
    if (NumLights >= MAX_LIGHTS)
    {
        base::glog << "Light limit is reached" << base::logwarn;
        throw std::runtime_error("light limit reached");
    }

    m_type = type;
    m_lightId = NumLights++;

    m_diffuse.r = m_diffuse.g = m_diffuse.b = 0.5f;
    m_specular.r = m_specular.g = m_specular.b = 1.0f;

    m_constantAttenuation = 1.0f;
    m_linearAttenuation = 1.0f;
    m_quadraticAttenuation = 1.0f;
}

LightComponent::LightComponent(const Json::Value &root)
    // FIXME:
    // Support other light types!
    : LightComponent(Type::DIRECTIONAL)
{
    auto typeStr = root["type"].asString();
    if (typeStr != "directional")
    {
        base::glog << "Can not create light component. Unsupported light type" << typeStr << base::logwarn;
        throw std::runtime_error("Unsupported light type");
    }

    setDirection(utils::jsonGetValueWithDefault(root["direction"], m_direction));
    setDiffuseIntensity(utils::jsonGetValueWithDefault(root["diffuse"], m_diffuse));
    setSpecularIntensity(utils::jsonGetValueWithDefault(root["specular"], m_specular));
}

LightComponent::~LightComponent()
{
    NumLights--;
}

void LightComponent::setDirection(const glm::vec3 &dir)
{
    if (m_type != Type::DIRECTIONAL)
    {
        base::glog << "Trying to set direction to not directional light" << base::logwarn;
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

} }