#pragma once

#include <base/EngineController.h>
#include <render/RenderManager.h>
#include <scene/SceneManager.h>
#include <resources/ResourceManager.h>
#include <resources/FileSystem.h>
#include <gui/GuiManager.h>
#include <physics/PhysicsManager.h>
#include <audio/AudioManager.h>

#define g_engineController df3d::base::EngineController::getInstance()

#define g_sceneManager df3d::base::EngineController::getInstance()->getSceneManager()
#define g_resourceManager df3d::base::EngineController::getInstance()->getResourceManager()
#define g_fileSystem df3d::base::EngineController::getInstance()->getFileSystem()
#define g_renderManager df3d::base::EngineController::getInstance()->getRenderManager()
#define g_guiManager df3d::base::EngineController::getInstance()->getGuiManager()
#define g_physics df3d::base::EngineController::getInstance()->getPhysicsManager()
#define g_physicsWorld df3d::base::EngineController::getInstance()->getPhysicsManager()->getWorld()
#define g_audioManager df3d::base::EngineController::getInstance()->getAudioManager()
