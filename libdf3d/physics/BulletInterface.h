#pragma once

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include <render/Vertex.h>

FWD_MODULE_CLASS(render, RenderPass)

namespace df3d { namespace physics {

class BulletDebugDraw : public btIDebugDraw
{
    shared_ptr<render::RenderPass> m_pass;

    std::vector<render::Vertex_3p2tx4c> m_vertexData;

public:
    BulletDebugDraw();
    ~BulletDebugDraw();

    void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) override;
    void drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color) override;
    void reportErrorWarning(const char *warningString) override;
    void draw3dText(const btVector3 &location, const char *textString) override;
    void setDebugMode(int debugMode) override;
    int getDebugMode() const override;

    void flushRenderOperations();
};

} }
