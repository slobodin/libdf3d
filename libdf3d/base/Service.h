#pragma once

#include <base/EngineController.h>
#include <base/DebugConsole.h>
#include <render/RenderManager.h>
#include <scene/SceneManager.h>
#include <resources/ResourceManager.h>
#include <resources/FileSystem.h>
#include <resources/ResourceFactory.h>
#include <gui/GuiManager.h>
#include <physics/PhysicsManager.h>
#include <audio/AudioManager.h>

namespace df3d {

class DF3D_DLL df3dServices
{
    friend class base::EngineController;

    df3dServices(scene::SceneManager &sceneMgr,
                 resources::ResourceManager &resourceMgr,
                 resources::FileSystem &filesystem,
                 render::RenderManager &renderMgr,
                 gui::GuiManager &guiMgr,
                 physics::PhysicsManager &physicsMgr,
                 btDynamicsWorld &physicsWorld,
                 audio::AudioManager &audioManager,
                 base::DebugConsole *debugConsole);

public:
    scene::SceneManager &sceneMgr;
    resources::ResourceManager &resourceMgr;
    resources::FileSystem &filesystem;
    render::RenderManager &renderMgr;
    gui::GuiManager &guiMgr;
    physics::PhysicsManager &physicsMgr;
    btDynamicsWorld &physicsWorld;
    audio::AudioManager &audioManager;

    base::DebugConsole *console;
};

DF3D_DLL df3dServices& gsvc();

}
