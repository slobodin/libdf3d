#pragma once

#include "libdf3d_dll.h"
#include "df3d_config.h"

#include <base/Common.h>

#include <utils/ConcurrentQueue.h>
#include <utils/Utils.h>
#include <utils/JsonUtils.h>
#include <utils/MathUtils.h>
#include <utils/MeshUtils.h>
#include <utils/SceneSerializer.h>
#include <utils/Dict.h>

#include <base/Service.h>
#include <base/InputEvents.h>
#include <base/DebugConsole.h>

#include <platform/AppDelegate.h>
#include <platform/Storage.h>

#include <components/AudioComponent.h>
#include <components/LightComponent.h>
#include <components/MeshComponent.h>
#include <components/ParticleSystemComponent.h>
#include <components/PhysicsComponent.h>
#include <components/TransformComponent.h>
#include <components/TextMeshComponent.h>
#include <components/Sprite2DComponent.h>
#include <components/ComponentFactory.h>

#include <scene/Node.h>
#include <scene/NodeFactory.h>
#include <scene/Frustum.h>
#include <scene/Camera.h>
#include <scene/Scene.h>
#include <scene/SceneManager.h>
#include <scene/bounding_volumes/BoundingVolume.h>
#include <scene/bounding_volumes/AABB.h>
#include <scene/bounding_volumes/BoundingSphere.h>
#include <scene/bounding_volumes/OBB.h>
#include <scene/bounding_volumes/ConvexHull.h>

#include <render/Material.h>
#include <render/MaterialLib.h>
#include <render/Technique.h>
#include <render/RenderPass.h>
#include <render/RenderStats.h>
#include <render/Viewport.h>
#include <render/Vertex.h>
#include <render/MeshData.h>

#include <resources/FileSystem.h>
#include <resources/FileDataSource.h>

#include <gui/GuiManager.h>

#include <physics/PhysicsManager.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include <audio/AudioManager.h>
