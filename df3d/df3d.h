#pragma once

#include <df3d/Common.h>

#include <df3d/lib/memory/Allocator.h>
#include <df3d/lib/assert/Assert.h>
#include <df3d/lib/containers/ConcurrentQueue.h>
#include <df3d/lib/containers/PodArray.h>
#include <df3d/lib/os/PlatformFile.h>
#include <df3d/lib/os/PlatformStorage.h>
#include <df3d/lib/os/PlatformUtils.h>
#include <df3d/lib/Utils.h>
#include <df3d/lib/JsonUtils.h>
#include <df3d/lib/math/MathUtils.h>
#include <df3d/lib/math/Frustum.h>
#include <df3d/lib/Dict.h>
#include <df3d/lib/Handles.h>
#include <df3d/lib/Any.h>
#include <df3d/lib/Id.h>

#include <df3d/engine/EngineController.h>
#include <df3d/engine/ConfigVariable.h>
#include <df3d/engine/EngineCVars.h>
#include <df3d/engine/TimeManager.h>

#include <df3d/platform/AppDelegate.h>
#include <df3d/platform/LocalNotification.h>

#include <df3d/engine/2d/Sprite2DComponentProcessor.h>

#include <df3d/engine/3d/Camera.h>
#include <df3d/engine/3d/SceneGraphComponentProcessor.h>
#include <df3d/engine/3d/StaticMeshComponentProcessor.h>

#include <df3d/lib/math/BoundingVolume.h>
#include <df3d/lib/math/AABB.h>
#include <df3d/lib/math/BoundingSphere.h>
#include <df3d/lib/math/OBB.h>

#include <df3d/engine/render/RenderCommon.h>
#include <df3d/engine/render/Material.h>
#include <df3d/engine/render/Viewport.h>
#include <df3d/engine/render/Vertex.h>
#include <df3d/engine/render/RenderQueue.h>
#include <df3d/engine/render/IRenderBackend.h>
#include <df3d/engine/render/RenderManager.h>

#include <df3d/engine/input/InputEvents.h>
#include <df3d/engine/input/InputManager.h>

#include <df3d/engine/io/FileSystemHelpers.h>

#include <df3d/engine/resources/GpuProgramResource.h>
#include <df3d/engine/resources/MaterialResource.h>
#include <df3d/engine/resources/MeshResource.h>
#include <df3d/engine/resources/ParticleSystemResource.h>
#include <df3d/engine/resources/TextureResource.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/ResourceDataSource.h>
#include <df3d/engine/resources/ResourceFileSystem.h>
#include <df3d/engine/resources/EntityResource.h>

#include <df3d/engine/gui/GuiManager.h>

#include <df3d/engine/physics/PhysicsHelpers.h>
#include <df3d/engine/physics/PhysicsComponentProcessor.h>
#include <df3d/engine/physics/PhysicsComponentCreationParams.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include <df3d/game/FPSCamera.h>
#include <df3d/game/Entity.h>
#include <df3d/game/ComponentDataHolder.h>
#include <df3d/game/World.h>
#include <df3d/game/WorldRenderingParams.h>
#include <df3d/game/WorldSize.h>
#include <df3d/game/TagComponentProcessor.h>
#include <df3d/game/EntityComponentLoader.h>

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
