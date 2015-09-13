#include "df3d_pch.h"
#include "Service.h"

namespace df3d {

df3dServices::df3dServices(scene::SceneManager &sceneMgr,
                           resources::ResourceManager &resourceMgr,
                           resources::FileSystem &filesystem,
                           render::RenderManager &renderMgr,
                           gui::GuiManager &guiMgr,
                           physics::PhysicsManager &physicsMgr,
                           btDynamicsWorld &physicsWorld,
                           audio::AudioManager &audioManager)
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

df3dServices& gsvc()
{
    assert(base::EngineController::instance().initialized());
    return base::EngineController::instance().svc();
}

}
