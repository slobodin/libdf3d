cmake_minimum_required(VERSION 3.1)

project(libdf3d)

include(${PROJECT_SOURCE_DIR}/cmake/libdf3d.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/cotire.cmake)

if (WINDOWS AND NOT CMAKE_PREFIX_PATH)
    set (CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/third-party/prebuilt_win32" CACHE PATH "third-party")
endif()

# FIXME: choose abi
if (ANDROID AND NOT CMAKE_PREFIX_PATH)
    set (CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/third-party/prebuilt_android/armeabi-v7a" CACHE PATH "third-party")
endif()

if (IOS AND NOT CMAKE_PREFIX_PATH)
    set (CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/third-party/prebuilt_ios" CACHE PATH "third-party")
endif()

if (DF3D_DESKTOP)
    add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/glfw)
endif()
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/bullet)
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/ogg)
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/vorbis)
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/spark/src)
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/libRocket/Build)
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/squirrel)

OPTION(ALSOFT_DLOPEN OFF)
OPTION(ALSOFT_WERROR OFF)
OPTION(ALSOFT_UTILS OFF)
OPTION(ALSOFT_NO_CONFIG_UTIL OFF)
OPTION(ALSOFT_EXAMPLES OFF)
OPTION(ALSOFT_TESTS OFF)
OPTION(ALSOFT_CONFIG OFF)
OPTION(ALSOFT_HRTF_DEFS OFF)
OPTION(ALSOFT_INSTALL OFF)
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/openal-soft)
if(NOT WIN32)
    SET(DF3D_OPENAL_LIBNAME openal)
else()
    SET(DF3D_OPENAL_LIBNAME OpenAL32)
endif()

if (NOT ANDROID AND NOT IOS)
    option(DF3D_BUILD_SHARED_LIB "Build shared df3d library" ON)
endif()

set(libdf3d_HEADER_LIST
    ${PROJECT_SOURCE_DIR}/libdf3d/df3d.h
    ${PROJECT_SOURCE_DIR}/libdf3d/df3d_pch.h
    ${PROJECT_SOURCE_DIR}/libdf3d/df3d_config.h
    ${PROJECT_SOURCE_DIR}/libdf3d/libdf3d_dll.h
    ${PROJECT_SOURCE_DIR}/libdf3d/2d/Sprite2DComponentProcessor.h
    ${PROJECT_SOURCE_DIR}/libdf3d/3d/Camera.h
    ${PROJECT_SOURCE_DIR}/libdf3d/3d/DebugDrawComponentProcessor.h
    ${PROJECT_SOURCE_DIR}/libdf3d/3d/Frustum.h
    ${PROJECT_SOURCE_DIR}/libdf3d/3d/Light.h
    ${PROJECT_SOURCE_DIR}/libdf3d/3d/StaticMeshComponentProcessor.h
    ${PROJECT_SOURCE_DIR}/libdf3d/3d/SceneGraphComponentProcessor.h
    ${PROJECT_SOURCE_DIR}/libdf3d/audio/AudioBuffer.h
    ${PROJECT_SOURCE_DIR}/libdf3d/audio/AudioComponentProcessor.h
    ${PROJECT_SOURCE_DIR}/libdf3d/audio/AudioManager.h
    ${PROJECT_SOURCE_DIR}/libdf3d/audio/impl/OpenALCommon.h
    ${PROJECT_SOURCE_DIR}/libdf3d/base/Common.h
    ${PROJECT_SOURCE_DIR}/libdf3d/base/DebugConsole.h
    ${PROJECT_SOURCE_DIR}/libdf3d/base/EngineController.h
    ${PROJECT_SOURCE_DIR}/libdf3d/base/EngineInitParams.h
    ${PROJECT_SOURCE_DIR}/libdf3d/base/FrameStats.h
    ${PROJECT_SOURCE_DIR}/libdf3d/base/Log.h
    ${PROJECT_SOURCE_DIR}/libdf3d/base/StringTable.h
    ${PROJECT_SOURCE_DIR}/libdf3d/base/TimeManager.h
    ${PROJECT_SOURCE_DIR}/libdf3d/game/ComponentDataHolder.h
    ${PROJECT_SOURCE_DIR}/libdf3d/game/Entity.h
    ${PROJECT_SOURCE_DIR}/libdf3d/game/EntityComponentLoader.h
    ${PROJECT_SOURCE_DIR}/libdf3d/game/EntityComponentProcessor.h
    ${PROJECT_SOURCE_DIR}/libdf3d/game/FPSCamera.h
    ${PROJECT_SOURCE_DIR}/libdf3d/game/TagComponentProcessor.h
    ${PROJECT_SOURCE_DIR}/libdf3d/game/World.h
    ${PROJECT_SOURCE_DIR}/libdf3d/game/WorldRenderingParams.h
    ${PROJECT_SOURCE_DIR}/libdf3d/game/WorldSize.h
    ${PROJECT_SOURCE_DIR}/libdf3d/game/impl/EntityLoader.h
    ${PROJECT_SOURCE_DIR}/libdf3d/game/impl/EntityManager.h
    ${PROJECT_SOURCE_DIR}/libdf3d/game/impl/WorldLoader.h
    ${PROJECT_SOURCE_DIR}/libdf3d/game/impl/AudioComponentLoader.h
    ${PROJECT_SOURCE_DIR}/libdf3d/game/impl/MeshComponentLoader.h
    ${PROJECT_SOURCE_DIR}/libdf3d/game/impl/PhysicsComponentLoader.h
    ${PROJECT_SOURCE_DIR}/libdf3d/game/impl/Sprite2DComponentLoader.h
    ${PROJECT_SOURCE_DIR}/libdf3d/game/impl/SceneGraphComponentLoader.h
    ${PROJECT_SOURCE_DIR}/libdf3d/game/impl/VfxComponentLoader.h
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/GuiManager.h
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/RocketRefWrapper.h
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/impl/RocketInterface.h
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/impl/RocketKeyCodesAdapter.h
    ${PROJECT_SOURCE_DIR}/libdf3d/input/InputEvents.h
    ${PROJECT_SOURCE_DIR}/libdf3d/input/InputManager.h
    ${PROJECT_SOURCE_DIR}/libdf3d/io/FileDataSource.h
    ${PROJECT_SOURCE_DIR}/libdf3d/io/FileSystem.h
    ${PROJECT_SOURCE_DIR}/libdf3d/io/FileSystemHelpers.h
    ${PROJECT_SOURCE_DIR}/libdf3d/io/Storage.h
    ${PROJECT_SOURCE_DIR}/libdf3d/math/AABB.h
    ${PROJECT_SOURCE_DIR}/libdf3d/math/BoundingSphere.h
    ${PROJECT_SOURCE_DIR}/libdf3d/math/BoundingVolume.h
    ${PROJECT_SOURCE_DIR}/libdf3d/math/ConvexHull.h
    ${PROJECT_SOURCE_DIR}/libdf3d/math/OBB.h
    ${PROJECT_SOURCE_DIR}/libdf3d/particlesys/ParticleSystemComponentProcessor.h
    ${PROJECT_SOURCE_DIR}/libdf3d/particlesys/ParticleSystemUtils.h
    ${PROJECT_SOURCE_DIR}/libdf3d/particlesys/impl/SparkInterface.h
    ${PROJECT_SOURCE_DIR}/libdf3d/particlesys/impl/ParticleSystemLoader.h
    ${PROJECT_SOURCE_DIR}/libdf3d/physics/PhysicsComponentCreationParams.h
    ${PROJECT_SOURCE_DIR}/libdf3d/physics/PhysicsComponentProcessor.h
    ${PROJECT_SOURCE_DIR}/libdf3d/physics/PhysicsHelpers.h
    ${PROJECT_SOURCE_DIR}/libdf3d/physics/impl/BulletInterface.h
    ${PROJECT_SOURCE_DIR}/libdf3d/platform/AppDelegate.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/GpuProgram.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/GpuProgramState.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/GpuProgramUniform.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Material.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/MaterialLib.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/MeshData.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/OpenGLCommon.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RendererBackend.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderCommon.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderManager.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderOperation.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderPass.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderQueue.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderTarget.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderTargetScreen.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderTargetTexture.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Shader.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Technique.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Texture.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Texture2D.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/TextureCube.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Vertex.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/VertexIndexBuffer.h
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Viewport.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/Resource.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/ResourceFactory.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/ResourceManager.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resource_loaders/AudioLoaders.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resource_loaders/MaterialLibLoaders.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resource_loaders/MeshLoaders.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resource_loaders/TextureLoaders.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resource_loaders/impl/AudioLoader_ogg.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resource_loaders/impl/AudioLoader_wav.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resource_loaders/impl/MeshLoader_dfmesh.h
    ${PROJECT_SOURCE_DIR}/libdf3d/resource_loaders/impl/MeshLoader_obj.h
    ${PROJECT_SOURCE_DIR}/libdf3d/script/ScriptManager.h
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/Any.h
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/ConcurrentQueue.h
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/Dict.h
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/JsonUtils.h
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/MathUtils.h
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/MeshUtils.h
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/NonCopyable.h
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/ThreadPool.h
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/Utils.h
)

set(libdf3d_SRC_LIST
    ${PROJECT_SOURCE_DIR}/libdf3d/2d/Sprite2DComponentProcessor.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/3d/Camera.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/3d/DebugDrawComponentProcessor.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/3d/Frustum.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/3d/Light.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/3d/StaticMeshComponentProcessor.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/3d/SceneGraphComponentProcessor.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/audio/AudioBuffer.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/audio/AudioComponentProcessor.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/audio/AudioManager.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/audio/impl/OpenALCommon.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/base/Common.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/base/DebugConsole.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/base/impl/ConsoleWindowRmlSource.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/base/EngineController.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/base/Log.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/base/StringTable.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/base/TimeManager.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/game/FPSCamera.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/game/TagComponentProcessor.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/game/World.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/game/WorldRenderingParams.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/game/impl/EntityLoader.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/game/impl/EntityManager.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/game/impl/WorldLoader.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/GuiManager.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/impl/RocketInterface.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/gui/impl/RocketKeyCodesAdapter.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/input/InputEvents.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/input/InputManager.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/io/FileSystem.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/io/Storage.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/math/AABB.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/math/BoundingSphere.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/math/BoundingVolume.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/math/ConvexHull.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/math/OBB.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/particlesys/ParticleSystemComponentProcessor.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/particlesys/ParticleSystemUtils.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/particlesys/impl/SparkInterface.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/particlesys/impl/ParticleSystemLoader.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/physics/PhysicsComponentCreationParams.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/physics/PhysicsComponentProcessor.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/physics/impl/BulletInterface.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/GpuProgram.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/GpuProgramState.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/GpuProgramUniform.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Material.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/MaterialLib.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/MeshData.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/OpenGLCommon.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RendererBackend.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderCommon.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderManager.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderOperation.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderPass.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderQueue.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderTargetScreen.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/RenderTargetTexture.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Shader.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Technique.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Texture.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Texture2D.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/TextureCube.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Vertex.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/VertexIndexBuffer.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/render/Viewport.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/Resource.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/ResourceFactory.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resources/ResourceManager.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resource_loaders/AudioLoaders.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resource_loaders/MaterialLibLoaders.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resource_loaders/MeshLoaders.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resource_loaders/TextureLoaders.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resource_loaders/impl/AudioLoader_ogg.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resource_loaders/impl/AudioLoader_wav.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resource_loaders/impl/MeshLoader_dfmesh.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/resource_loaders/impl/MeshLoader_obj.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/script/ScriptManager.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/Dict.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/JsonUtils.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/MathUtils.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/MeshUtils.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/ThreadPool.cpp
    ${PROJECT_SOURCE_DIR}/libdf3d/utils/Utils.cpp
)

add_definitions(-DBULLET_FORCE_NO_ALIGNMENT)

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
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/desktop_common/glfwApplication.h
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/desktop_common/glfwKeyCodes.h
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/desktop_common/FileDataSourceDesktop.h
    )
    list(APPEND libdf3d_SRC_LIST
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/desktop_common/glfwApplication.cpp
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/desktop_common/glfwKeyCodes.cpp
        ${PROJECT_SOURCE_DIR}/libdf3d/platform/desktop_common/FileDataSourceDesktop.cpp
    )
endif()

if (DF3D_DESKTOP)
    find_package(GLEW REQUIRED)
    find_package(OpenGL REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
    include_directories(${OPENGL_INCLUDE_DIR})
endif()

include_directories(${PROJECT_SOURCE_DIR}/
                    ${PROJECT_SOURCE_DIR}/third-party/
                    ${PROJECT_SOURCE_DIR}/third-party/bullet/src
                    ${PROJECT_SOURCE_DIR}/third-party/spark/include
                    ${PROJECT_SOURCE_DIR}/third-party/libRocket/Include
                    ${PROJECT_SOURCE_DIR}/third-party/glfw/include
                    ${PROJECT_SOURCE_DIR}/third-party/squirrel/include
                    ${PROJECT_SOURCE_DIR}/third-party/openal-soft/include)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd\"4251\" /wd\"4457\" /wd\"4458\" /wd\"4138\"")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -DGLEW_STATIC)
endif()

if (DF3D_BUILD_SHARED_LIB)
    set(DF3D_LIBRARY_TYPE SHARED)
    add_definitions(-DLIBDF3D_EXPORTS -DJSON_DLL_BUILD -DDF3D_SHARED_LIBRARY)
else()
    set(DF3D_LIBRARY_TYPE STATIC)
endif()

add_library(libdf3d ${DF3D_LIBRARY_TYPE}
    ${libdf3d_HEADER_LIST}
    ${libdf3d_SRC_LIST}
    ${third_party_SRC_FILES}
)

target_link_libraries(libdf3d
    LinearMath
    BulletCollision
    BulletDynamics
    ogg
    spark
    vorbis
    RocketCore
    RocketControls
    RocketDebugger
    squirrel)

if (DF3D_DESKTOP)
    target_link_libraries(libdf3d glfw ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${DF3D_OPENAL_LIBNAME})
endif()

if (WINDOWS_PHONE)
    target_link_libraries(libdf3d ${PREBUILT_LIBRARIES_PATH}/libEGL.lib)
    target_link_libraries(libdf3d ${PREBUILT_LIBRARIES_PATH}/libGLESv2.lib)
    target_link_libraries(libdf3d ${PREBUILT_LIBRARIES_PATH}/OpenAL.wp8.lib)
endif()

if (ANDROID)
    target_link_libraries(libdf3d OpenSLES ${DF3D_OPENAL_LIBNAME} GLESv2 log android)
endif()

set_target_properties(libdf3d PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "$(PROJECT_SOURCE_DIR)/libdf3d/df3d_pch.h")
cotire(libdf3d)
