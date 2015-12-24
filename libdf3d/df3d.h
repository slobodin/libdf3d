#pragma once

#include "libdf3d_dll.h"
#include "df3d_config.h"

#include <base/Common.h>

#include <utils/ConcurrentQueue.h>
#include <utils/Utils.h>
#include <utils/JsonUtils.h>
#include <utils/MathUtils.h>
#include <utils/MeshUtils.h>
#include <utils/Dict.h>

#include <base/EngineController.h>
#include <base/DebugConsole.h>
#include <base/TimeManager.h>

#include <platform/AppDelegate.h>

#include <3d/Frustum.h>
#include <3d/Camera.h>
#include <3d/TransformComponentProcessor.h>
#include <3d/StaticMeshComponentProcessor.h>

#include <math/BoundingVolume.h>
#include <math/AABB.h>
#include <math/BoundingSphere.h>
#include <math/OBB.h>
#include <math/ConvexHull.h>

#include <render/Material.h>
#include <render/MaterialLib.h>
#include <render/Technique.h>
#include <render/RenderPass.h>
#include <render/RenderStats.h>
#include <render/Viewport.h>
#include <render/Vertex.h>
#include <render/MeshData.h>

#include <input/InputEvents.h>
#include <input/InputManager.h>

#include <io/FileSystem.h>
#include <io/FileDataSource.h>
#include <io/Storage.h>

#include <resources/ResourceManager.h>
#include <resources/ResourceFactory.h>

#include <gui/GuiManager.h>

#include <physics/PhysicsHelpers.h>
#include <physics/PhysicsComponentProcessor.h>
#include <physics/PhysicsComponentCreationParams.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include <audio/AudioManager.h>
#include <audio/AudioComponentProcessor.h>

#include <game/DebugNameComponentProcessor.h>
#include <game/FPSCamera.h>
#include <game/Entity.h>
#include <game/ComponentDataHolder.h>
#include <game/World.h>
#include <game/WorldRenderingParams.h>

#include <particlesys/ParticleSystemComponentProcessor.h>

#include <script/ScriptManager.h>
