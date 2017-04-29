#pragma once

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <df3d/engine/render/RenderCommon.h>
#include <df3d/engine/render/Vertex.h>

namespace df3d { 

class RenderPass;
struct RenderQueue;

class BulletDebugDraw : public btIDebugDraw
{
    unique_ptr<RenderPass> m_pass;

    static const size_t MAX_VERTICES = (2 << 19);
    VertexBufferHandle m_vertexBuffer;
    VertexData m_vertexData;
    int m_currentVertex = 0;

public:
    BulletDebugDraw();
    ~BulletDebugDraw();

    void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) override;
    void drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color) override;
    void reportErrorWarning(const char *warningString) override;
    void draw3dText(const btVector3 &location, const char *textString) override;
    void setDebugMode(int debugMode) override;
    int getDebugMode() const override;

    void clean();
    void flushRenderOperations(RenderQueue *ops);
};

}
