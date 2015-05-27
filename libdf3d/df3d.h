#pragma once

#include "libdf3d_dll.h"
#include "df3d_config.h"

#include <base/TypeDefs.h>
#include <base/MacroDefs.h>
#include <base/Common.h>

#include <utils/ConcurrentQueue.h>
#include <utils/Utils.h>
#include <utils/JsonHelpers.h>
#include <utils/SceneSerializer.h>

#include <base/SystemsMacro.h>
#include <base/Log.h>
#include <base/InputEvents.h>

#include <platform/Application.h>

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

#include <resources/FileSystem.h>
#include <resources/FileDataSource.h>

#include <gui/GuiManager.h>

#include <physics/PhysicsManager.h>
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <audio/AudioManager.h>
