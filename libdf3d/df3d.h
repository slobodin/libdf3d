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

#include <components/AudioComponent.h>
#include <components/LightComponent.h>
#include <components/MeshComponent.h>
#include <components/ParticleSystemComponent.h>
#include <components/PhysicsComponent.h>
#include <components/TransformComponent.h>
#include <components/TextMeshComponent.h>
#include <components/Sprite2DComponent.h>

#include <scene/Node.h>
#include <scene/Frustum.h>
#include <scene/Camera.h>
#include <scene/Scene.h>
#include <scene/SceneManager.h>
#include <scene/Entity.h>
#include <scene/World.h>
#include <scene/TransformComponentProcessor.h>

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
#include <render/StaticMeshComponentProcessor.h>

#include <input/InputEvents.h>
#include <input/InputManager.h>

#include <io/FileSystem.h>
#include <io/FileDataSource.h>
#include <io/Storage.h>

#include <resources/ResourceManager.h>
#include <resources/ResourceFactory.h>

#include <gui/GuiManager.h>

#include <physics/PhysicsManager.h>
#include <physics/PhysicsComponentProcessor.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include <audio/AudioManager.h>
#include <audio/AudioComponentProcessor.h>

#include <game/ComponentFactory.h>
#include <game/NodeFactory.h>
#include <game/SceneFactory.h>

#include <particlesys/ParticleSystemComponentProcessor.h>
