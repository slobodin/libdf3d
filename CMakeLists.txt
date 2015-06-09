cmake_minimum_required(VERSION 2.8)

project(libdf3d)

include(${PROJECT_SOURCE_DIR}/cmake/libdf3d.cmake)

add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/src/bullet)
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/src/libogg)
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/src/libvorbis)
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/src/spark)

set(libdf3d_HEADER_LIST
    ${PROJECT_SOURCE_DIR}/libdf3d/df3d.h
    ${PROJECT_SOURCE_DIR}/libdf3d/df3d_pch.h
    ${PROJECT_SOURCE_DIR}/libdf3d/df3d_config.h
    ${PROJECT_SOURCE_DIR}/libdf3d/libdf3d_dll.h
    ${PROJECT_SOURCE_DIR}/libdf3d/audio/AudioBuffer.h
    ${PROJECT_SOURCE_DIR}/libdf3d/audio/AudioManager.h
    ${PROJECT_SOURCE_DIR}/libdf3d/audio/OpenALCommon.h
    ${PROJECT_SOURCE_DIR}/libdf3d/base/Common.h
    ${PROJECT_SOURCE_DIR}/libdf3d/base/EngineController.h
    ${PROJECT_SOURCE_DIR}/libdf3d/base/EngineInitParams.h
    ${PROJECT_SOURCE_DIR}/libdf3d/base/InputEvents.h
    ${PROJECT_SOURCE_DIR}/libdf3d/base/Log.h
    ${PROJECT_SOURCE_DIR}/libdf3d/base/MacroDefs.h
    ${PROJECT_SOURCE_DIR}/libdf3d/base/SystemsMacro.h
    ${PROJECT_SOURCE_DIR}/libdf3d/base/ThreadPool.h
    ${PROJECT_SOURCE_DIR}/libdf3d/base/TypeDefs.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/AudioComponent.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/DebugDrawComponent.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/LightComponent.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/MeshComponent.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/NodeComponent.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/ParticleSystemComponent.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/PhysicsComponent.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/Sprite2DComponent.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/TextMeshComponent.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/TransformComponent.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/serializers/AudioComponentSerializer.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/serializers/ComponentSerializer.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/serializers/DebugDrawComponentSerializer.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/serializers/LightComponentSerializer.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/serializers/MeshComponentSerializer.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/serializers/ParticleSystemComponentSerializer.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/serializers/PhysicsComponentSerializer.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/serializers/Sprite2DComponentSerializer.h
    ${PROJECT_SOURCE_DIR}/libdf3d/components/serializers/TransformComponentSerializer.h
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/DebugOverlayWindow.h
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/GuiManager.h
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/RocketInterface.h
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/RocketIntrusivePtr.h
    ${PROJECT_SOURCE_DIR}/libdf3d/particlesys/SparkInterface.h
    ${PROJECT_SOURCE_DIR}/libdf3d/physics/BulletInterface.h
    ${PROJECT_SOURCE_DIR}/libdf3d/physics/PhysicsManager.h
    ${PROJECT_SOURCE_DIR}/libdf3d/platform/AppDelegate.h
    ${PROJECT_SOURCE_DIR}/libdf3d/platform/Application.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/GpuProgram.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/GpuProgramState.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/GpuProgramUniform.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Material.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/MaterialLib.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/MeshData.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/OpenGLCommon.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Renderer.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderingCapabilities.h
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
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Texture2D.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/TextureCube.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Vertex.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/VertexIndexBuffer.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Viewport.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/FileDataSource.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/FileSystem.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/Resource.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/ResourceDecoder.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/ResourceManager.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/decoders/DecoderTexture.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/decoders/DecoderMTL.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/decoders/DecoderOBJ.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/decoders/DecoderOGG.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/decoders/DecoderTerrain.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/decoders/DecoderWAV.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/Camera.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/Frustum.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/Node.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/Scene.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/SceneManager.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/SceneManagerListener.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/WorldSize.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/bounding_volumes/AABB.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/bounding_volumes/BoundingSphere.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/bounding_volumes/BoundingVolume.h
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/bounding_volumes/OBB.h
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/ConcurrentQueue.h
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/JsonHelpers.h
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/MeshIndexer.h
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/SceneSerializer.h
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/Utils.h
)

set(libdf3d_SRC_LIST
    ${PROJECT_SOURCE_DIR}/libdf3d/audio/AudioBuffer.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/audio/AudioManager.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/audio/OpenALCommon.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/base/Common.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/base/EngineController.cpp
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
    ${PROJECT_SOURCE_DIR}/libdf3d/components/Sprite2DComponent.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/components/TextMeshComponent.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/components/TransformComponent.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/components/serializers/AudioComponentSerializer.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/components/serializers/DebugDrawComponentSerializer.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/components/serializers/ComponentSerializer.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/components/serializers/LightComponentSerializer.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/components/serializers/MeshComponentSerializer.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/components/serializers/ParticleSystemComponentSerializer.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/components/serializers/PhysicsComponentSerializer.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/components/serializers/Sprite2DComponentSerializer.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/components/serializers/TransformComponentSerializer.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/DebugOverlayWindow.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/GuiManager.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/RocketInterface.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/RocketIntrusivePtr.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/particlesys/SparkInterface.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/physics/BulletInterface.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/physics/PhysicsManager.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/platform/Application.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/GpuProgram.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/GpuProgramState.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/GpuProgramUniform.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Material.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/MaterialLib.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/MeshData.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/OpenGLCommon.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Renderer.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderingCapabilities.cpp
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
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Texture2D.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/TextureCube.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Vertex.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/VertexIndexBuffer.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Viewport.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/FileDataSource.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/FileSystem.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/Resource.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/ResourceDecoder.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/ResourceManager.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/decoders/DecoderTexture.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/decoders/DecoderMTL.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/decoders/DecoderOBJ.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/decoders/DecoderOGG.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/decoders/DecoderTerrain.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/decoders/DecoderWAV.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/Camera.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/Frustum.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/Node.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/Scene.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/SceneManager.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/bounding_volumes/AABB.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/bounding_volumes/BoundingSphere.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/bounding_volumes/BoundingVolume.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/scene/bounding_volumes/OBB.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/JsonHelpers.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/MeshIndexer.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/SceneSerializer.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/Utils.cpp
)

add_definitions(-DLIBDF3D_EXPORTS -DJSON_DLL_BUILD)

if (NOT CMAKE_PREFIX_PATH)
    set (CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/third-party" CACHE PATH "third-party")
endif()

if (WINDOWS)
    set(PREBUILT_LIBRARIES_PATH ${PROJECT_SOURCE_DIR}/third-party/prebuilt/win32)
elseif (WINDOWS_PHONE)
    set(PREBUILT_LIBRARIES_PATH ${PROJECT_SOURCE_DIR}/third-party/prebuilt/wp_81)
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm200 /wd\"4251\" /wd\"4457\" /wd\"4458\"")

    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)

    set(PRECOMPILED_H ${PROJECT_SOURCE_DIR}/libdf3d/df3d_pch.h)
    set(PRECOMPILED_CPP ${PROJECT_SOURCE_DIR}/libdf3d/df3d_pch.cpp)

    set_source_files_properties(${PRECOMPILED_CPP} PROPERTIES COMPILE_FLAGS "/Ycdf3d_pch.h" OBJECT_OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/df3d_pch.pch")
    set_source_files_properties(${libdf3d_SRC_LIST} PROPERTIES COMPILE_FLAGS "/Yudf3d_pch.h" OBJECT_DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/df3d_pch.pch")
endif(MSVC)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
endif(CMAKE_COMPILER_IS_GNUCXX)

set(third_party_SRC_FILES
    ${PROJECT_SOURCE_DIR}/third-party/src/lib_json/jsoncpp.cpp
)

if (WINDOWS)
    add_definitions(-DWIN32 -D_WINDOWS -DDF3D_WINDOWS -DBULLET_FORCE_NO_ALIGNMENT)

    list(APPEND libdf3d_HEADER_LIST
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/windows/CrashHandler.h
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/windows/WindowsApplication.h
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/windows/WindowsKeyCodes.h
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/windows/WindowsFileSystemHelpers.h
    )
    list(APPEND libdf3d_SRC_LIST
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/windows/CrashHandler.cpp
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/windows/WindowsApplication.cpp
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/windows/WindowsKeyCodes.cpp
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/windows/WindowsFileSystemHelpers.cpp
    )
endif()

if (WINDOWS_PHONE)
    include_directories(${PROJECT_SOURCE_DIR}/third-party/include/angle)

    add_definitions(-DDF3D_WINDOWS_PHONE -DBULLET_FORCE_NO_ALIGNMENT)

    list(APPEND libdf3d_HEADER_LIST
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/windows_phone/WindowsPhoneApplication.h
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/windows_phone/WindowsPhoneFileSystemHelpers.h
    )
    list(APPEND libdf3d_SRC_LIST
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/windows_phone/WindowsPhoneApplication.cpp
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/windows_phone/WindowsPhoneFileSystemHelpers.cpp
    )
endif()

find_package(Boost REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/libdf3d
                    ${PROJECT_SOURCE_DIR}/third-party/include
                    ${Boost_INCLUDE_DIRS})

if (WINDOWS)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
endif(WINDOWS)

add_library(libdf3d SHARED
    ${libdf3d_HEADER_LIST}
    ${libdf3d_SRC_LIST}
    ${third_party_SRC_FILES}
    ${PROJECT_SOURCE_DIR}/libdf3d/df3d_pch.cpp
)

target_link_libraries(libdf3d
    BulletPhysics
    libogg
    libspark
    libvorbis)

if (WINDOWS)
    target_link_libraries(libdf3d ${OPENGL_LIBRARIES})
    target_link_libraries(libdf3d ${GLEW_LIBRARIES})
    target_link_libraries(libdf3d ${PREBUILT_LIBRARIES_PATH}/OpenAL32.lib)
endif(WINDOWS)

if (WINDOWS OR WINDOWS_PHONE)
    function(link_with_rocket library_name)
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_link_libraries(libdf3d ${PREBUILT_LIBRARIES_PATH}/${library_name}_d.lib)
        else()
            target_link_libraries(libdf3d ${PREBUILT_LIBRARIES_PATH}/${library_name}.lib)
        endif()
    endfunction()

    link_with_rocket(RocketControls)
    link_with_rocket(RocketCore)
    link_with_rocket(RocketDebugger)
endif()

if (WINDOWS_PHONE)
    target_link_libraries(libdf3d ${PREBUILT_LIBRARIES_PATH}/libEGL.lib)
    target_link_libraries(libdf3d ${PREBUILT_LIBRARIES_PATH}/libGLESv2.lib)
    target_link_libraries(libdf3d ${PREBUILT_LIBRARIES_PATH}/OpenAL.wp8.lib)
endif()
