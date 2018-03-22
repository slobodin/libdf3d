#include "AnimatedMeshComponentProcessor.h"

#include <df3d/engine/EngineController.h>
#include <df3d/engine/render/RenderOperation.h>
#include <df3d/engine/render/RenderQueue.h>
#include <df3d/engine/TimeManager.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/MeshResource.h>
#include <df3d/engine/resources/MaterialResource.h>
#include <df3d/engine/3d/SceneGraphComponentProcessor.h>
#include <df3d/game/World.h>
#include <glm/gtx/transform.hpp>

namespace df3d {

void AnimatedMeshComponentProcessor::update()
{
    auto &sceneGr = m_world.sceneGraph();

    auto dt = svc().timer().getFrameDelta(df3d::TIME_CHANNEL_GAME);
    for (auto &compData : m_data.rawData())
    {
        compData.holderWorldTransform = sceneGr.getWorldTransform(compData.holder);
        if (compData.animating)
        {
            compData.timer += dt;
            if (compData.timer >= (1.0f / 60.0f))
            {
                compData.timer = 0.0f;
                //compData.frameCounter = (compData.frameCounter + 1) % compData.maxFrames;
                compData.frameCounter++;
            }
        }
    }
}

void AnimatedMeshComponentProcessor::drawNode(AnimatedMeshNode *node, RenderQueue *ops, Data &data, glm::mat4 parentTransform)
{
    if (!node)
        return;

    if (node->meshIdx >= 0)
    {
        auto tech = data.materials.at(node->meshIdx).getCurrentTechnique();
        DF3D_ASSERT(tech);

        const auto &meshPart = data.meshParts[node->meshIdx];

        for (auto &pass : tech->passes)
        {
            RenderOperation op;

            op.vertexBuffer = meshPart.vertexBuffer;
            op.indexBuffer = meshPart.indexBuffer;
            op.numberOfElements = meshPart.numberOfElements;
            op.worldTransform = parentTransform;
            op.passProps = &pass;

            auto bucketID = op.passProps->preferredBucket;
            if (bucketID == RQ_BUCKET_COUNT)
                bucketID = RQ_BUCKET_NOT_LIT;

            ops->rops[bucketID].push_back(op);
        }
    }

    for (const auto &child : node->children)
    {
        glm::vec3 pos;
        glm::quat q;
        glm::mat4 t = node->transform;
        if (data.frameCounter < node->animation.size())
        {
            pos = node->animation.at(data.frameCounter).position;
            q = node->animation.at(data.frameCounter).orientation;
            t = glm::translate(pos) * glm::toMat4(q);
        }
        else if (data.animating && !node->animation.empty())
        {
            pos = node->animation.back().position;
            q = node->animation.back().orientation;
            t = glm::translate(pos) * glm::toMat4(q);
        }

        drawNode(child.get(), ops, data, parentTransform * t);
    }
}

void AnimatedMeshComponentProcessor::draw(RenderQueue *ops)
{
    for (auto &compData : m_data.rawData())
        drawNode(compData.root.get(), ops, compData, compData.holderWorldTransform.combined);
}

AnimatedMeshComponentProcessor::AnimatedMeshComponentProcessor(World &world)
    : m_world(world)
{

}

AnimatedMeshComponentProcessor::~AnimatedMeshComponentProcessor()
{

}

void AnimatedMeshComponentProcessor::startAnimation(Entity e)
{
    m_data.getData(e).animating = true;
}

void AnimatedMeshComponentProcessor::add(Entity e, Id meshResource, int framesCount)
{
    DF3D_ASSERT_MESS(!m_data.contains(e), "An entity already has an animated mesh component");

    auto &rmgr = svc().resourceManager();
    if (auto mesh = rmgr.getResource<AnimatedMeshResource>(meshResource))
    {
        Data data;

        data.holder = e;
        data.maxFrames = framesCount;
        data.holderWorldTransform = m_world.sceneGraph().getWorldTransform(e);
        data.meshParts = mesh->meshParts;
        data.materials.resize(data.meshParts.size());
        data.root = mesh->root;

        if (!mesh->materialLibResourceId.empty())
        {
            auto materialLib = rmgr.getResource<MaterialLibResource>(mesh->materialLibResourceId);
            DF3D_ASSERT(materialLib);
            DF3D_ASSERT(mesh->meshParts.size() == mesh->materialNames.size());

            size_t idx = 0;
            for (const auto &mtlName : mesh->materialNames)
            {
                auto material = materialLib->getMaterial(mtlName);
                if (material)
                    data.materials[idx] = *material;
                ++idx;
            }
        }

        m_data.add(e, data);
    }
    else
        DFLOG_WARN("Failed to add static mesh to an entity. Resource '%s' is not loaded", meshResource.toString().c_str());
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
