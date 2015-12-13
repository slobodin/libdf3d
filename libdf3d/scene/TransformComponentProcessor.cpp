#include "df3d_pch.h"
#include "TransformComponentProcessor.h"

namespace df3d {

TransformComponentProcessor::TransformComponentProcessor()
{

}

TransformComponentProcessor::~TransformComponentProcessor()
{

}

void TransformComponentProcessor::setPosition(ComponentInstance comp, const glm::vec3 &newPosition)
{

}

void TransformComponentProcessor::setPosition(ComponentInstance comp, float x, float y, float z)
{

}

void TransformComponentProcessor::setScale(ComponentInstance comp, const glm::vec3 &newScale)
{

}

void TransformComponentProcessor::setScale(ComponentInstance comp, float x, float y, float z)
{

}

void TransformComponentProcessor::setScale(ComponentInstance comp, float uniform)
{

}

void TransformComponentProcessor::setOrientation(ComponentInstance comp, const glm::quat &newOrientation)
{

}

void TransformComponentProcessor::setOrientation(ComponentInstance comp, const glm::vec3 &eulerAngles, bool rads)
{

}

void TransformComponentProcessor::setTransformation(ComponentInstance comp, const glm::mat4 &tr)
{

}

void TransformComponentProcessor::translate(ComponentInstance comp, const glm::vec3 &v)
{

}

void TransformComponentProcessor::translate(ComponentInstance comp, float x, float y, float z)
{

}

void TransformComponentProcessor::scale(ComponentInstance comp, const glm::vec3 &v)
{

}

void TransformComponentProcessor::scale(ComponentInstance comp, float x, float y, float z)
{

}

void TransformComponentProcessor::scale(ComponentInstance comp, float uniform)
{

}

void TransformComponentProcessor::rotateYaw(ComponentInstance comp, float yaw, bool rads)
{

}

void TransformComponentProcessor::rotatePitch(ComponentInstance comp, float pitch, bool rads)
{

}

void TransformComponentProcessor::rotateRoll(ComponentInstance comp, float roll, bool rads)
{

}

void TransformComponentProcessor::rotateAxis(ComponentInstance comp, float angle, const glm::vec3 &axis, bool rads)
{

}

glm::vec3 TransformComponentProcessor::getPosition(ComponentInstance comp, bool includeParent)
{
    return glm::vec3();
}

glm::vec3 TransformComponentProcessor::getScale(ComponentInstance comp, bool includeParent)
{
    return glm::vec3();
}

glm::quat TransformComponentProcessor::getOrientation(ComponentInstance comp, bool includeParent)
{
    return glm::quat();
}

glm::mat4 TransformComponentProcessor::getTransformation(ComponentInstance comp)
{
    return glm::mat4();
}

glm::vec3 TransformComponentProcessor::getRotation(ComponentInstance comp, bool rads, bool includeParent)
{
    return glm::vec3();
}

ComponentInstance TransformComponentProcessor::add(Entity e)
{
    return ComponentInstance();
}

void TransformComponentProcessor::remove(Entity e)
{

}

ComponentInstance TransformComponentProcessor::lookup(Entity e)
{
    return ComponentInstance();
}

}
