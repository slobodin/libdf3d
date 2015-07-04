cmake_minimum_required(VERSION 3.1)

project(libdf3d)

include(${PROJECT_SOURCE_DIR}/cmake/libdf3d.cmake)

if (WINDOWS AND NOT CMAKE_PREFIX_PATH)
    set (CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/third-party/prebuilt_win32" CACHE PATH "third-party")
endif()

if (ANDROID AND NOT CMAKE_PREFIX_PATH)
    set (CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/third-party/prebuilt_android/armeabi-v7a" CACHE PATH "third-party")
endif()

add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/bullet/src)
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/ogg)
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/vorbis)
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/spark/src)
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/libRocket/Build)

if (DF3D_DESKTOP)
    add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/glfw)
endif()

if (ANDROID)
    include(${PROJECT_SOURCE_DIR}/cmake/AndroidNdkModules.cmake)
endif()

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
    ${PROJECT_SOURCE_DIR}/libdf3d/platform/Storage.h
    ${PROJECT_SOURCE_DIR}/libdf3d/platform/FileSystemHelpers.h
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

add_definitions(-DLIBDF3D_EXPORTS -DJSON_DLL_BUILD -DBULLET_FORCE_NO_ALIGNMENT)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm200 /wd\"4251\" /wd\"4457\" /wd\"4458\"")

    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)

    set(PRECOMPILED_H ${PROJECT_SOURCE_DIR}/libdf3d/df3d_pch.h)
    set(PRECOMPILED_CPP ${PROJECT_SOURCE_DIR}/libdf3d/df3d_pch.cpp)

    set_source_files_properties(${PRECOMPILED_CPP} PROPERTIES COMPILE_FLAGS "/Ycdf3d_pch.h" OBJECT_OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/df3d_pch.pch")
    set_source_files_properties(${libdf3d_SRC_LIST} PROPERTIES COMPILE_FLAGS "/Yudf3d_pch.h" OBJECT_DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/df3d_pch.pch")
endif()

set(third_party_SRC_FILES
    ${PROJECT_SOURCE_DIR}/third-party/json/jsoncpp.cpp
)

if (WINDOWS)
    list(APPEND libdf3d_HEADER_LIST
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/windows/CrashHandler.h
    )
    list(APPEND libdf3d_SRC_LIST
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/windows/CrashHandler.cpp
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/windows/Storage.cpp
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/windows/FileSystemHelpers.cpp
    )
endif()

if (WINDOWS_PHONE)
    include_directories(${PROJECT_SOURCE_DIR}/third-party/include/angle)

    list(APPEND libdf3d_HEADER_LIST
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/windows_phone/WindowsPhoneApplication.h
    )
    list(APPEND libdf3d_SRC_LIST
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/windows_phone/Storage.cpp
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/windows_phone/WindowsPhoneApplication.cpp
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/windows_phone/FileSystemHelpers.cpp
    )
endif()

if (ANDROID)
    list(APPEND libdf3d_HEADER_LIST
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/android/FileDataSourceAndroid.h
    )
    list(APPEND libdf3d_SRC_LIST
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/android/Storage.cpp
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/android/FileSystemHelpers.cpp
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/android/AndroidBridge.cpp
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/android/FileDataSourceAndroid.cpp
    )
endif()

if (LINUX)
    list(APPEND libdf3d_SRC_LIST
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/linux/Storage.cpp
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/linux/FileSystemHelpers.cpp
    )
endif()

if (MACOSX)
    list(APPEND libdf3d_SRC_LIST
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/macosx/Storage.cpp
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/macosx/FileSystemHelpers.cpp
    )
endif()

if (DF3D_DESKTOP)
    list(APPEND libdf3d_HEADER_LIST
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/desktop_common/FileDataSourceDesktop.h
    )
    list(APPEND libdf3d_SRC_LIST
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/desktop_common/glfwApplication.cpp
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/desktop_common/FileDataSourceDesktop.cpp
    )
endif()

if (ANDROID)
    find_host_package(OpenAL REQUIRED)
    find_host_package(Boost REQUIRED)
    include_directories(${OPENAL_INCLUDE_DIR})
else()
    find_package(Boost REQUIRED)
endif()

if (DF3D_DESKTOP)
    find_package(GLEW REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(OpenAL REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
    include_directories(${OPENGL_INCLUDE_DIR})
    include_directories(${OPENAL_INCLUDE_DIR})
endif()

include_directories(${PROJECT_SOURCE_DIR}/libdf3d
                    ${PROJECT_SOURCE_DIR}/third-party/
                    ${PROJECT_SOURCE_DIR}/third-party/bullet/include
                    ${PROJECT_SOURCE_DIR}/third-party/spark/include
                    ${PROJECT_SOURCE_DIR}/third-party/libRocket/Include
                    ${PROJECT_SOURCE_DIR}/third-party/glfw/include
                    ${Boost_INCLUDE_DIRS})

if (ANDROID)
    set(LIBRARY_TYPE STATIC)
else()
    set(LIBRARY_TYPE SHARED)
endif()

add_library(libdf3d ${LIBRARY_TYPE}
    ${libdf3d_HEADER_LIST}
    ${libdf3d_SRC_LIST}
    ${third_party_SRC_FILES}
    ${PROJECT_SOURCE_DIR}/libdf3d/df3d_pch.cpp
)

target_link_libraries(libdf3d
    BulletPhysics
    ogg
    spark
    vorbis
    RocketCore
    RocketControls
    RocketDebugger)

if (DF3D_DESKTOP)
    target_link_libraries(libdf3d glfw ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${OPENAL_LIBRARY})
endif()

if (WINDOWS_PHONE)
    target_link_libraries(libdf3d ${PREBUILT_LIBRARIES_PATH}/libEGL.lib)
    target_link_libraries(libdf3d ${PREBUILT_LIBRARIES_PATH}/libGLESv2.lib)
    target_link_libraries(libdf3d ${PREBUILT_LIBRARIES_PATH}/OpenAL.wp8.lib)
endif()

if (ANDROID)
    target_link_libraries(libdf3d OpenSLES ${OPENAL_LIBRARY} GLESv2 log android)
endif()
