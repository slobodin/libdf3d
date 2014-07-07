#pragma once

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <render/RenderOperation.h>

namespace df3d { namespace physics {

class BulletDebugDraw : public btIDebugDraw
{
    render::RenderOperation m_linesOp;

public:
    BulletDebugDraw();
    ~BulletDebugDraw();

    void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color);
    void drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color);
    void reportErrorWarning(const char *warningString);
    void draw3dText(const btVector3 &location, const char *textString);
    void setDebugMode(int debugMode);
    int getDebugMode() const;

    void flushRenderOperations();
};

} }