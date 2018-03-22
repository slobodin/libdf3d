#include "AnimatedMeshComponentProcessor.h"

namespace df3d {

void AnimatedMeshComponentProcessor::update()
{
}

void AnimatedMeshComponentProcessor::draw(RenderQueue *ops)
{
}

AnimatedMeshComponentProcessor::AnimatedMeshComponentProcessor()
{

}

AnimatedMeshComponentProcessor::~AnimatedMeshComponentProcessor()
{

}

void AnimatedMeshComponentProcessor::add(Entity e, Id meshResource)
{
    DF3D_ASSERT_MESS(!m_data.contains(e), "An entity already has an animated mesh component");
}

void AnimatedMeshComponentProcessor::remove(Entity e)
{
    m_data.remove(e);
}

bool AnimatedMeshComponentProcessor::has(Entity e)
{
    return m_data.contains(e);
}

}
