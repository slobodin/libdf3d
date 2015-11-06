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

class DF3D_DLL Service
{
    friend class EngineController;

    Service(SceneManager &sceneMgr,
            ResourceManager &resourceMgr,
            FileSystem &filesystem,
            RenderManager &renderMgr,
            GuiManager &guiMgr,
            PhysicsManager &physicsMgr,
            btDynamicsWorld &physicsWorld,
            AudioManager &audioManager);

public:
    SceneManager &sceneMgr;
    ResourceManager &resourceMgr;
    FileSystem &filesystem;
    RenderManager &renderMgr;
    GuiManager &guiMgr;
    PhysicsManager &physicsMgr;
    btDynamicsWorld &physicsWorld;
    AudioManager &audioManager;

    DebugConsole *console;
};

DF3D_DLL Service& svc();

}
