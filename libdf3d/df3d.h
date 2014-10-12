#pragma once

#include "libdf3d_dll.h"
#include "df3d_config.h"

#include <utils/Utils.h>
#include <utils/JsonHelpers.h>

#include <base/MacroDefs.h>
#include <base/Common.h>
#include <base/Controller.h>
#include <base/AppDelegate.h>
#include <base/Log.h>
#include <base/InputEvents.h>

#include <components/AudioComponent.h>
#include <components/LightComponent.h>
#include <components/MeshComponent.h>
#include <components/ParticleSystemComponent.h>
#include <components/PhysicsComponent.h>
#include <components/TransformComponent.h>
#include <components/TextMeshComponent.h>

#include <scene/Node.h>
#include <scene/Camera.h>
#include <scene/FPSCamera.h>
#include <scene/Scene.h>
#include <scene/SceneManager.h>
#include <scene/bounding_volumes/BoundingVolume.h>
#include <scene/bounding_volumes/AABB.h>
#include <scene/bounding_volumes/BoundingSphere.h>
#include <scene/bounding_volumes/OBB.h>

#include <render/Material.h>
#include <render/MaterialLib.h>
#include <render/Technique.h>
#include <render/RenderPass.h>
#include <render/RenderStats.h>
#include <render/Viewport.h>

#include <resources/ResourceManager.h>
#include <resources/FileSystem.h>
#include <resources/FileDataSource.h>

#include <gui/RocketIntrusivePtr.h>
#include <gui/GuiManager.h>
#include <Rocket/Core.h>
#include <Rocket/Controls.h>

#include <scripting/ScriptManager.h>
#if defined(DF3D_USES_PYTHON)
#include <scripting/PythonUpdateProxy.h>
#include <scripting/PythonInputProxy.h>
#endif

#include <physics/PhysicsManager.h>
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <audio/AudioManager.h>

#include <SDL.h>
#undef main