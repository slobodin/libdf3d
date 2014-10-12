cmake_minimum_required(VERSION 2.8)

project(libdf3d)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third-party/src/bullet)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third-party/src/libogg)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third-party/src/libvorbis)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third-party/src/spark)

set(libdf3d_HEADER_LIST
    ${PROJECT_SOURCE_DIR}/libdf3d/df3d.h
    ${PROJECT_SOURCE_DIR}/libdf3d/df3d_pch.h
    ${PROJECT_SOURCE_DIR}/libdf3d/df3d_config.h
    ${PROJECT_SOURCE_DIR}/libdf3d/libdf3d_dll.h
    ${PROJECT_SOURCE_DIR}/libdf3d/audio/AudioBuffer.h
    ${PROJECT_SOURCE_DIR}/libdf3d/audio/AudioManager.h
    ${PROJECT_SOURCE_DIR}/libdf3d/audio/OpenALCommon.h
    ${PROJECT_SOURCE_DIR}/libdf3d/base/AppDelegate.h
    ${PROJECT_SOURCE_DIR}/libdf3d/base/Common.h
    ${PROJECT_SOURCE_DIR}/libdf3d/base/Controller.h
    ${PROJECT_SOURCE_DIR}/libdf3d/base/InputEvents.h
    ${PROJECT_SOURCE_DIR}/libdf3d/base/Log.h
    ${PROJECT_SOURCE_DIR}/libdf3d/base/MacroDefs.h
    ${PROJECT_SOURCE_DIR}/libdf3d/base/ThreadPool.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/AudioComponent.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/DebugDrawComponent.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/LightComponent.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/MeshComponent.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/NodeComponent.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/ParticleSystemComponent.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/PhysicsComponent.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/TextMeshComponent.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/TransformComponent.h
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/DebugOverlayWindow.h
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/GuiManager.h
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/RocketInterface.h
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/RocketIntrusivePtr.h
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/RocketKeyBindings.h
    ${PROJECT_SOURCE_DIR}/libdf3d/particlesys/SparkInterface.h
    ${PROJECT_SOURCE_DIR}/libdf3d/physics/BulletInterface.h
    ${PROJECT_SOURCE_DIR}/libdf3d/physics/PhysicsManager.h
    ${PROJECT_SOURCE_DIR}/libdf3d/platform/Application.h
    ${PROJECT_SOURCE_DIR}/libdf3d/platform/SDLApplication.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/GpuProgram.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/GpuProgramState.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/GpuProgramUniform.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Image.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Material.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/MaterialLib.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/MeshData.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/OpenGLCommon.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Renderer.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderManager.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderOperation.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderPass.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderQueue.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderStats.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderTarget.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderTargetScreen.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderTargetTexture.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Shader.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/SubMesh.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Technique.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Texture.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Vertex.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/VertexIndexBuffer.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Viewport.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/FileDataSource.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/FileSystem.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/Resource.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/ResourceDecoder.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/ResourceManager.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/decoders/DecoderImage.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/decoders/DecoderMTL.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/decoders/DecoderOBJ.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/decoders/DecoderOGG.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/decoders/DecoderTerrain.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/decoders/DecoderWAV.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/Camera.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/DebugDrawAABBNode.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/DebugDrawOBBNode.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/FPSCamera.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/Node.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/Scene.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/SceneManager.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/SceneManagerListener.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/WorldSize.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/bounding_volumes/AABB.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/bounding_volumes/BoundingSphere.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/bounding_volumes/BoundingVolume.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/bounding_volumes/OBB.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scripting/ScriptManager.h
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/JsonHelpers.h
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/MeshIndexer.h
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/ParticleSystemLoader.h
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/PhysicsLoader.h
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/SceneLoader.h
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/Utils.h
)

set(libdf3d_SRC_LIST
    ${PROJECT_SOURCE_DIR}/libdf3d/audio/AudioBuffer.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/audio/AudioManager.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/audio/OpenALCommon.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/base/Common.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/base/Controller.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/base/InputEvents.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/base/Log.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/base/ThreadPool.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/components/AudioComponent.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/components/DebugDrawComponent.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/components/LightComponent.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/components/MeshComponent.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/components/NodeComponent.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/components/ParticleSystemComponent.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/components/PhysicsComponent.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/components/TextMeshComponent.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/components/TransformComponent.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/DebugOverlayWindow.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/GuiManager.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/RocketInterface.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/RocketIntrusivePtr.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/RocketKeyBindings.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/particlesys/SparkInterface.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/physics/BulletInterface.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/physics/PhysicsManager.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/platform/Application.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/platform/SDLApplication.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/GpuProgram.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/GpuProgramState.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/GpuProgramUniform.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Image.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Material.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/MaterialLib.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/MeshData.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/OpenGLCommon.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Renderer.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderManager.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderOperation.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderPass.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderQueue.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderTargetScreen.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderTargetTexture.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Shader.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/SubMesh.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Technique.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Texture.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Vertex.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/VertexIndexBuffer.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Viewport.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/FileDataSource.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/FileSystem.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/Resource.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/ResourceDecoder.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/ResourceManager.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/decoders/DecoderImage.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/decoders/DecoderMTL.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/decoders/DecoderOBJ.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/decoders/DecoderOGG.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/decoders/DecoderTerrain.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/decoders/DecoderWAV.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/Camera.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/DebugDrawAABBNode.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/DebugDrawOBBNode.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/FPSCamera.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/Node.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/Scene.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/SceneManager.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/bounding_volumes/AABB.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/bounding_volumes/BoundingSphere.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/bounding_volumes/BoundingVolume.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/bounding_volumes/OBB.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/scripting/ScriptManager.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/JsonHelpers.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/MeshIndexer.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/ParticleSystemLoader.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/PhysicsLoader.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/SceneLoader.cpp
)

add_definitions(-DLIBDF3D_EXPORTS -DJSON_DLL_BUILD)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm200 /wd\"4251\"")

    add_definitions(-DWIN32 -D_WINDOWS -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)

    set(PRECOMPILED_CPP ${PROJECT_SOURCE_DIR}/libdf3d/df3d_pch.cpp)
    set_source_files_properties(${PRECOMPILED_CPP} PROPERTIES COMPILE_FLAGS "/Ycdf3d_pch.h")

    foreach(src_file ${libdf3d_SRC_LIST})
        set_source_files_properties(${src_file} PROPERTIES COMPILE_FLAGS "/Yudf3d_pch.h")
    endforeach(src_file ${libdf3d_SRC_LIST})

    list(APPEND libdf3d_SRC_LIST ${PRECOMPILED_CPP})
endif(MSVC)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
endif(CMAKE_COMPILER_IS_GNUCXX)

list(APPEND libdf3d_SRC_LIST
    ${PROJECT_SOURCE_DIR}/third-party/src/lib_json/json_reader.cpp
    ${PROJECT_SOURCE_DIR}/third-party/src/lib_json/json_value.cpp
    ${PROJECT_SOURCE_DIR}/third-party/src/lib_json/json_writer.cpp
)

if (WIN32)
    find_package(GLEW REQUIRED)
endif(WIN32)

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf>=2.0)
find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(Boost REQUIRED)

add_library(libdf3d SHARED ${libdf3d_SRC_LIST} ${libdf3d_HEADER_LIST})

include_directories(${PROJECT_SOURCE_DIR}/libdf3d
                    ${PROJECT_SOURCE_DIR}/third-party/include
                    ${SDL2_INCLUDE_DIRS}
                    ${Boost_INCLUDE_DIRS})

if (WIN32)
    include_directories(${GLEW_INCLUDE_DIR} ${OPENAL_INCLUDE_DIR})
endif(WIN32)

target_link_libraries(libdf3d 
    BulletPhysics 
    libogg
    libspark 
    libvorbis 
    ${SDL2_LIBRARY} 
    ${SDL2IMAGE_LIBRARY} 
    ${GLEW_LIBRARY} 
    ${SDL2TTF_LIBRARY} 
    ${OPENGL_LIBRARIES} 
    ${OPENAL_LIBRARY})

if (WIN32)
    target_link_libraries(libdf3d ${PROJECT_SOURCE_DIR}/third-party/lib/win32/RocketControls.lib)
    target_link_libraries(libdf3d ${PROJECT_SOURCE_DIR}/third-party/lib/win32/RocketCore.lib)
    target_link_libraries(libdf3d ${PROJECT_SOURCE_DIR}/third-party/lib/win32/RocketDebugger.lib)
endif(WIN32)
