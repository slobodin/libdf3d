#include "df3d_pch.h"
#include "LightComponentSerializer.h"

#include <components/LightComponent.h>
#include <utils/JsonHelpers.h>

namespace df3d { namespace components { namespace serializers {

void load(components::LightComponent *component, const Json::Value &root)
{
    //auto typeStr = root["type"].asString();
    //if (typeStr != "directional")
    //{
    //    return;
    //}

    component->setDirection(utils::jsonGetValueWithDefault(root["direction"], component->getDirection()));
    component->setDiffuseIntensity(utils::jsonGetValueWithDefault(root["diffuse"], component->getDiffuseColor()));
    component->setSpecularIntensity(utils::jsonGetValueWithDefault(root["specular"], component->getSpecularColor()));
}

Json::Value save(const components::LightComponent *component)
{
    Json::Value result;

    result["type"] = "directional"; // FIXME:
    result["direction"] = utils::glmToJson(component->getDirection());
    result["diffuse"] = utils::glmToJson(component->getDiffuseColor());
    result["specular"] = utils::glmToJson(component->getSpecularColor());

    return result;
}

} } }