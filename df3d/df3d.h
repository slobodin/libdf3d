#pragma once

#include "libdf3d_dll.h"
#include "df3d_config.h"

#include <df3d/Common.h>

#include <df3d/lib/assert/Assert.h>
#include <df3d/lib/containers/ConcurrentQueue.h>
#include <df3d/lib/Utils.h>
#include <df3d/lib/JsonUtils.h>
#include <df3d/lib/math/MathUtils.h>
#include <df3d/lib/math/Frustum.h>
#include <df3d/lib/MeshUtils.h>
#include <df3d/lib/Dict.h>
#include <df3d/lib/Any.h>

#include <df3d/engine/EngineController.h>
#include <df3d/engine/DebugConsole.h>
#include <df3d/engine/TimeManager.h>

#include <df3d/platform/AppDelegate.h>
#include <df3d/platform/Platform.h>
#include <df3d/platform/LocalNotification.h>

#include <df3d/engine/2d/Sprite2DComponentProcessor.h>

#include <df3d/engine/3d/Camera.h>
#include <df3d/engine/3d/SceneGraphComponentProcessor.h>
#include <df3d/engine/3d/StaticMeshComponentProcessor.h>

#include <df3d/lib/math/BoundingVolume.h>
#include <df3d/lib/math/AABB.h>
#include <df3d/lib/math/BoundingSphere.h>
#include <df3d/lib/math/OBB.h>
#include <df3d/lib/math/ConvexHull.h>

#include <df3d/engine/render/RenderCommon.h>
#include <df3d/engine/render/Material.h>
#include <df3d/engine/render/MaterialLib.h>
#include <df3d/engine/render/Technique.h>
#include <df3d/engine/render/RenderPass.h>
#include <df3d/engine/render/Viewport.h>
#include <df3d/engine/render/Vertex.h>
#include <df3d/engine/render/MeshData.h>
#include <df3d/engine/render/RenderQueue.h>
#include <df3d/engine/render/IRenderBackend.h>
#include <df3d/engine/render/RenderManager.h>

#include <df3d/engine/input/InputEvents.h>
#include <df3d/engine/input/InputManager.h>

#include <df3d/engine/io/FileSystem.h>
#include <df3d/engine/io/FileSystemHelpers.h>
#include <df3d/engine/io/FileDataSource.h>
#include <df3d/engine/io/Storage.h>
#include <df3d/engine/io/MemoryDataSource.h>

#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/ResourceFactory.h>

#include <df3d/engine/gui/GuiManager.h>

#include <df3d/engine/physics/PhysicsHelpers.h>
#include <df3d/engine/physics/PhysicsComponentProcessor.h>
#include <df3d/engine/physics/PhysicsComponentCreationParams.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include <df3d/engine/audio/AudioManager.h>
#include <df3d/engine/audio/AudioComponentProcessor.h>

#include <df3d/game/FPSCamera.h>
#include <df3d/game/Entity.h>
#include <df3d/game/ComponentDataHolder.h>
#include <df3d/game/World.h>
#include <df3d/game/WorldRenderingParams.h>
#include <df3d/game/WorldSize.h>
#include <df3d/game/TagComponentProcessor.h>

#include <df3d/engine/particlesys/ParticleSystemComponentProcessor.h>
#include <df3d/engine/particlesys/ParticleSystemUtils.h>

#include <df3d/engine/script/ScriptManager.h>

// turbobadger stuff
#include <tb_core.h>
#include <tb_language.h>
#include <tb_skin.h>
#include <tb_widgets.h>
#include <tb_message_window.h>
#include <tb_editfield.h>
#include <tb_font_renderer.h>
#include <tb_node_tree.h>
#include <tb_widgets_reader.h>
#include <tb_tab_container.h>
#include <tb_scroll_container.h>
#include <tb_inline_select.h>
#include <image/tb_image_widget.h>
#include <animation/tb_widget_animation.h>
