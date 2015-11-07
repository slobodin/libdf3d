#include "Service.h"

namespace df3d {

Service::Service(SceneManager &sceneMgr,
                 ResourceManager &resourceMgr,
                 FileSystem &filesystem,
                 RenderManager &renderMgr,
                 GuiManager &guiMgr,
                 PhysicsManager &physicsMgr,
                 btDynamicsWorld &physicsWorld,
                 AudioManager &audioManager)
    : sceneMgr(sceneMgr),
    resourceMgr(resourceMgr),
    filesystem(filesystem),
    renderMgr(renderMgr),
    guiMgr(guiMgr),
    physicsMgr(physicsMgr),
    physicsWorld(physicsWorld),
    audioManager(audioManager)
{

}

Service& svc()
{
    assert(EngineController::instance().initialized());
    return EngineController::instance().svc();
}

}
