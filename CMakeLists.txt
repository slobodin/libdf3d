cmake_minimum_required(VERSION 3.6)

project(libdf3d)

include(cmake/libdf3d.cmake)
include(cmake/cotire.cmake)

message(STATUS "DF3D_PLATFORM is " ${DF3D_PLATFORM})

if (DF3D_WINDOWS)
    set (CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/third-party/prebuilt_win32" CACHE PATH "third-party")
endif()

set(libdf3d_HEADER_LIST
    df3d_pch.h
    df3d/Common.h
    df3d/df3d.h
    df3d/df3d_config.h
    df3d/libdf3d_dll.h
    df3d/engine/ConfigVariable.h
    df3d/engine/EngineController.h
    df3d/engine/EngineCVars.h
    df3d/engine/EngineInitParams.h
    df3d/engine/TimeManager.h
    df3d/engine/2d/Sprite2DComponentProcessor.h
    df3d/engine/3d/Camera.h
    df3d/engine/3d/Light.h
    df3d/engine/3d/SceneGraphComponentProcessor.h
    df3d/engine/3d/StaticMeshComponentProcessor.h
    df3d/engine/gui/GuiManager.h
    df3d/engine/gui/TBInterface.h
    df3d/engine/input/InputEvents.h
    df3d/engine/input/InputManager.h
    df3d/engine/io/FileSystemHelpers.h
    df3d/engine/network/HttpClient.h
    df3d/engine/particlesys/ParticleSystemComponentProcessor.h
    df3d/engine/particlesys/ParticleSystemUtils.h
    df3d/engine/particlesys/SparkCommon.h
    df3d/engine/particlesys/SparkLineTrailRenderer.h
    df3d/engine/particlesys/SparkQuadRenderer.h
    df3d/engine/physics/btGImpactConvexDecompositionShape.h
    df3d/engine/physics/BulletInterface.h
    df3d/engine/physics/PhysicsComponentCreationParams.h
    df3d/engine/physics/PhysicsComponentProcessor.h
    df3d/engine/physics/PhysicsHelpers.h
    df3d/engine/render/GpuProgramSharedState.h
    df3d/engine/render/IRenderBackend.h
    df3d/engine/render/Material.h
    df3d/engine/render/MeshUtils.h
    df3d/engine/render/RenderCommon.h
    df3d/engine/render/RenderManager.h
    df3d/engine/render/RenderOperation.h
    df3d/engine/render/RenderQueue.h
    df3d/engine/render/Vertex.h
    df3d/engine/render/Viewport.h
    df3d/engine/render/gl/GLSLPreprocess.h
    df3d/engine/render/gl/RenderBackendGL.h
    df3d/engine/render/gl/embed_glsl/ambient_frag.h
    df3d/engine/render/gl/embed_glsl/ambient_vert.h
    df3d/engine/render/gl/embed_glsl/colored_frag.h
    df3d/engine/render/gl/embed_glsl/colored_vert.h
    df3d/engine/resources/EntityResource.h
    df3d/engine/resources/GpuProgramResource.h
    df3d/engine/resources/IResourceHolder.h
    df3d/engine/resources/MaterialResource.h
    df3d/engine/resources/MeshResource.h
    df3d/engine/resources/ParticleSystemResource.h
    df3d/engine/resources/ResourceDataSource.h
    df3d/engine/resources/ResourceFileSystem.h
    df3d/engine/resources/ResourceManager.h
    df3d/engine/resources/TextureResource.h
    df3d/engine/resources/loaders/MeshLoader_dfmesh.h
    df3d/engine/resources/loaders/MeshLoader_obj.h
    df3d/engine/resources/loaders/TextureLoader_pvrtc.h
    df3d/engine/resources/loaders/TextureLoader_stbi.h
    df3d/engine/resources/loaders/TextureLoader_webp.h
    df3d/engine/script/ScriptBindings.h
    df3d/engine/script/ScriptManager.h
    df3d/game/ComponentDataHolder.h
    df3d/game/Entity.h
    df3d/game/EntityComponentLoader.h
    df3d/game/EntityComponentProcessor.h
    df3d/game/FPSCamera.h
    df3d/game/TagComponentProcessor.h
    df3d/game/World.h
    df3d/game/WorldRenderingParams.h
    df3d/game/WorldSize.h
    df3d/game/impl/EntityLoader.h
    df3d/game/impl/MeshComponentLoader.h
    df3d/game/impl/PhysicsComponentLoader.h
    df3d/game/impl/SceneGraphComponentLoader.h
    df3d/game/impl/Sprite2DComponentLoader.h
    df3d/game/impl/VfxComponentLoader.h
    df3d/game/impl/WorldLoader.h
    df3d/lib/Any.h
    df3d/lib/Dict.h
    df3d/lib/Handles.h
    df3d/lib/Id.h
    df3d/lib/JsonUtils.h
    df3d/lib/Log.h
    df3d/lib/NonCopyable.h
    df3d/lib/ThreadPool.h
    df3d/lib/Utils.h
    df3d/lib/assert/Assert.h
    df3d/lib/containers/ConcurrentQueue.h
    df3d/lib/containers/PodArray.h
    df3d/lib/math/AABB.h
    df3d/lib/math/BoundingSphere.h
    df3d/lib/math/BoundingVolume.h
    df3d/lib/math/ConvexHull.h
    df3d/lib/math/Frustum.h
    df3d/lib/math/MathUtils.h
    df3d/lib/math/OBB.h
    df3d/lib/memory/Allocator.h
    df3d/lib/memory/MallocAllocator.h
    df3d/lib/memory/PoolAllocator.h
    df3d/lib/os/PlatformFile.h
    df3d/lib/os/PlatformStorage.h
    df3d/lib/os/PlatformUtils.h
    df3d/platform/AppDelegate.h
    df3d/platform/LocalNotification.h
    df3d/platform/InAppPurchase.h
)

set(libdf3d_SRC_LIST
    df3d_pch.cpp
    df3d/Common.cpp
    df3d/engine/EngineController.cpp
    df3d/engine/EngineCVars.cpp
    df3d/engine/TimeManager.cpp
    df3d/engine/2d/Sprite2DComponentProcessor.cpp
    df3d/engine/3d/Camera.cpp
    df3d/engine/3d/Light.cpp
    df3d/engine/3d/SceneGraphComponentProcessor.cpp
    df3d/engine/3d/StaticMeshComponentProcessor.cpp
    df3d/engine/gui/GuiManager.cpp
    df3d/engine/gui/TBInterface.cpp
    df3d/engine/input/InputEvents.cpp
    df3d/engine/input/InputManager.cpp
    df3d/engine/io/FileSystemHelpers.cpp
    df3d/engine/particlesys/ParticleSystemComponentProcessor.cpp
    df3d/engine/particlesys/ParticleSystemUtils.cpp
    df3d/engine/particlesys/SparkCommon.cpp
    df3d/engine/particlesys/SparkLineTrailRenderer.cpp
    df3d/engine/particlesys/SparkQuadRenderer.cpp
    df3d/engine/physics/btGImpactConvexDecompositionShape.cpp
    df3d/engine/physics/BulletInterface.cpp
    df3d/engine/physics/PhysicsComponentCreationParams.cpp
    df3d/engine/physics/PhysicsComponentProcessor.cpp
    df3d/engine/render/GpuProgramSharedState.cpp
    df3d/engine/render/Material.cpp
    df3d/engine/render/MeshUtils.cpp
    df3d/engine/render/RenderCommon.cpp
    df3d/engine/render/RenderManager.cpp
    df3d/engine/render/RenderOperation.cpp
    df3d/engine/render/RenderQueue.cpp
    df3d/engine/render/Vertex.cpp
    df3d/engine/render/Viewport.cpp
    df3d/engine/render/gl/GLSLPreprocess.cpp
    df3d/engine/render/gl/RenderBackendGL.cpp
    df3d/engine/resources/EntityResource.cpp
    df3d/engine/resources/GpuProgramResource.cpp
    df3d/engine/resources/MaterialResource.cpp
    df3d/engine/resources/MeshResource.cpp
    df3d/engine/resources/ParticleSystemResource.cpp
    df3d/engine/resources/ResourceDataSource.cpp
    df3d/engine/resources/ResourceFileSystem.cpp
    df3d/engine/resources/ResourceManager.cpp
    df3d/engine/resources/TextureResource.cpp
    df3d/engine/resources/loaders/MeshLoader_dfmesh.cpp
    df3d/engine/resources/loaders/MeshLoader_obj.cpp
    df3d/engine/resources/loaders/TextureLoader_pvrtc.cpp
    df3d/engine/resources/loaders/TextureLoader_stbi.cpp
    df3d/engine/resources/loaders/TextureLoader_webp.cpp
    df3d/engine/script/ScriptBindings.cpp
    df3d/engine/script/ScriptManager.cpp
    df3d/game/FPSCamera.cpp
    df3d/game/TagComponentProcessor.cpp
    df3d/game/World.cpp
    df3d/game/WorldRenderingParams.cpp
    df3d/game/impl/EntityLoader.cpp
    df3d/game/impl/WorldLoader.cpp
    df3d/lib/Dict.cpp
    df3d/lib/Handles.cpp
    df3d/lib/Id.cpp
    df3d/lib/JsonUtils.cpp
    df3d/lib/Log.cpp
    df3d/lib/ThreadPool.cpp
    df3d/lib/Utils.cpp
    df3d/lib/assert/Assert.cpp
    df3d/lib/memory/MallocAllocator.cpp
    df3d/lib/memory/PoolAllocator.cpp
    df3d/lib/math/AABB.cpp
    df3d/lib/math/BoundingSphere.cpp
    df3d/lib/math/ConvexHull.cpp
    df3d/lib/math/Frustum.cpp
    df3d/lib/math/MathUtils.cpp
    df3d/lib/math/OBB.cpp
    df3d/platform/AppDelegate.cpp
)

set(third_party_HEADER_FILES
        third-party/mikktspace/mikktspace.h
        third-party/json/json/json.h
        third-party/json/json/json-forwards.h
        )

set(third_party_SRC_FILES
        third-party/mikktspace/mikktspace.cpp
        third-party/json/jsoncpp.cpp
        )

if (DF3D_WINDOWS)
    list(APPEND libdf3d_HEADER_LIST
        df3d/platform/windows/CrashHandler.h
    )
    list(APPEND libdf3d_SRC_LIST
        df3d/platform/windows/CrashHandler.cpp
        df3d/platform/windows/InAppPurchase.cpp
    )
endif()

if (DF3D_ANDROID)
    list(APPEND libdf3d_SRC_LIST
        df3d/lib/os/PlatformFile_android.cpp
        df3d/lib/os/PlatformStorage_android.cpp
        df3d/lib/os/PlatformUtils_android.cpp

        df3d/platform/android/AndroidBridge.cpp
        df3d/platform/android/JNIHelpers.cpp
        df3d/platform/android/InAppPurchase.cpp
        df3d/platform/android/LocalNotification.cpp
        df3d/platform/android/HttpClient.cpp
    )
endif()

if (DF3D_IOS)
    list(APPEND libdf3d_HEADER_LIST
        df3d/platform/ios/AppDelegate.h
        df3d/platform/ios/GameViewController.h
    )
    list(APPEND libdf3d_SRC_LIST
        df3d/lib/os/PlatformFile_ios.mm
        df3d/lib/os/PlatformStorage_ios.mm
        df3d/lib/os/PlatformUtils_ios.mm

        df3d/platform/ios/AppDelegate.mm
        df3d/platform/ios/GameViewController.mm
        df3d/platform/ios/LocalNotification.mm
        df3d/platform/ios/InAppPurchase.mm
        df3d/platform/ios/HttpClient.mm
    )

    set_source_files_properties("df3d/engine/render/gl/RenderBackendGL.cpp" PROPERTIES COMPILE_FLAGS "-x objective-c++")
endif()

if (DF3D_DESKTOP)
    list(APPEND libdf3d_HEADER_LIST
        df3d/platform/desktop_common/glfwApplication.h
        df3d/platform/desktop_common/glfwKeyCodes.h
    )
    list(APPEND libdf3d_SRC_LIST
        df3d/lib/os/PlatformStorage_desktop.cpp
        df3d/lib/os/PlatformUtils_desktop.cpp
        df3d/lib/os/PlatformFile_desktop.cpp

        df3d/platform/desktop_common/glfwApplication.cpp
        df3d/platform/desktop_common/LocalNotification.cpp
        df3d/platform/desktop_common/glfwKeyCodes.cpp
        df3d/platform/desktop_common/HttpClient.cpp

        df3d/platform/windows/InAppPurchase.cpp
    )
endif()

if (DF3D_DESKTOP)
    find_package(GLEW REQUIRED)
    find_package(OpenGL REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
    include_directories(${OPENGL_INCLUDE_DIR})
    include_directories(${PROJECT_SOURCE_DIR}/third-party/glfw/include)
endif()

include_directories(${PROJECT_SOURCE_DIR}/
                    ${PROJECT_SOURCE_DIR}/third-party/
                    ${PROJECT_SOURCE_DIR}/third-party/bullet/src
                    ${PROJECT_SOURCE_DIR}/third-party/spark/include
                    ${PROJECT_SOURCE_DIR}/third-party/squirrel/include
                    ${CMAKE_CURRENT_BINARY_DIR}/third-party/zlib
                    ${PROJECT_SOURCE_DIR}/third-party/zlib
                    ${PROJECT_SOURCE_DIR}/third-party/webp
                    ${PROJECT_SOURCE_DIR}/third-party/turbobadger/src/tb)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd\"4251\" /wd\"4457\" /wd\"4458\" /wd\"4138\"")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
endif()

if (NOT (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    add_definitions(-DSCRAT_NO_ERROR_CHECKING)
    add_definitions(-DBT_NO_PROFILE)
endif()

add_library(libdf3d STATIC
    ${libdf3d_HEADER_LIST}
    ${libdf3d_SRC_LIST}
    ${third_party_HEADER_FILES}
    ${third_party_SRC_FILES}
)

if (DF3D_DESKTOP)
    add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/glfw)
endif()
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/bullet)
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/spark/src)
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/squirrel)
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/zlib)
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/webp)
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/freetype-2.6.5)

SET(FREETYPE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/third-party/freetype-2.6.5/include")
SET(FREETYPE_LIBRARY freetype)

SET(TB_BUILD_DEMO OFF CACHE BOOL "" FORCE)
SET(TB_GENERATE_CONFIG ON CACHE BOOL "" FORCE)
SET(TB_FONT_RENDERER_TBBF ON CACHE BOOL "" FORCE)
SET(TB_FONT_RENDERER_FREETYPE ON CACHE BOOL "" FORCE)
SET(TB_FONT_RENDERER_STB OFF CACHE BOOL "" FORCE)
SET(TB_IMAGE_LOADER_STB OFF CACHE BOOL "" FORCE)
SET(TB_RENDERER_BATCHER OFF CACHE BOOL "" FORCE)
SET(TB_RENDERER_GL OFF CACHE BOOL "" FORCE)
SET(TB_RENDERER_GLES_1 OFF CACHE BOOL "" FORCE)
SET(TB_UNIT_TESTING OFF CACHE BOOL "" FORCE)
SET(TB_IMAGE ON CACHE BOOL "" FORCE)
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/turbobadger)

# TODO: if build tools
if (DF3D_DESKTOP)
    add_subdirectory(${PROJECT_SOURCE_DIR}/tools/obj_to_dfmesh)
    # add_subdirectory(${PROJECT_SOURCE_DIR}/tools/glsl_preprocess)
endif()

target_link_libraries(libdf3d
    LinearMath
    BulletCollision
    BulletDynamics
    ConvexDecomposition
    spark
    squirrel
    ${FREETYPE_LIBRARY}
    zlibstatic
    TurboBadgerLib
    webp)

if (DF3D_DESKTOP)
    target_link_libraries(libdf3d glfw ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
endif()

if (DF3D_ANDROID)
    target_link_libraries(libdf3d OpenSLES GLESv2 log android)
endif()

if (DF3D_IOS)
    if (DF3D_TVOS)
        set(DEFAULT_TVOS_DEPL_TARGET "9.0")
        set_xcode_property(libdf3d TVOS_DEPLOYMENT_TARGET ${DEFAULT_TVOS_DEPL_TARGET})
        set_xcode_property(LinearMath TVOS_DEPLOYMENT_TARGET ${DEFAULT_TVOS_DEPL_TARGET})
        set_xcode_property(BulletCollision TVOS_DEPLOYMENT_TARGET ${DEFAULT_TVOS_DEPL_TARGET})
        set_xcode_property(BulletDynamics TVOS_DEPLOYMENT_TARGET ${DEFAULT_TVOS_DEPL_TARGET})
        set_xcode_property(ConvexDecomposition TVOS_DEPLOYMENT_TARGET ${DEFAULT_TVOS_DEPL_TARGET})
        set_xcode_property(spark TVOS_DEPLOYMENT_TARGET ${DEFAULT_TVOS_DEPL_TARGET})
        set_xcode_property(squirrel TVOS_DEPLOYMENT_TARGET ${DEFAULT_TVOS_DEPL_TARGET})
        set_xcode_property(${FREETYPE_LIBRARY} TVOS_DEPLOYMENT_TARGET ${DEFAULT_TVOS_DEPL_TARGET})
        set_xcode_property(zlibstatic TVOS_DEPLOYMENT_TARGET ${DEFAULT_TVOS_DEPL_TARGET})
        set_xcode_property(TurboBadgerLib TVOS_DEPLOYMENT_TARGET ${DEFAULT_TVOS_DEPL_TARGET})
        set_xcode_property(webp TVOS_DEPLOYMENT_TARGET ${DEFAULT_TVOS_DEPL_TARGET})
    else()
        set(DEFAULT_IOS_DEPL_TARGET "8.0")
        set_xcode_property(libdf3d IPHONEOS_DEPLOYMENT_TARGET ${DEFAULT_IOS_DEPL_TARGET})
        set_xcode_property(LinearMath IPHONEOS_DEPLOYMENT_TARGET ${DEFAULT_IOS_DEPL_TARGET})
        set_xcode_property(BulletCollision IPHONEOS_DEPLOYMENT_TARGET ${DEFAULT_IOS_DEPL_TARGET})
        set_xcode_property(BulletDynamics IPHONEOS_DEPLOYMENT_TARGET ${DEFAULT_IOS_DEPL_TARGET})
        set_xcode_property(ConvexDecomposition IPHONEOS_DEPLOYMENT_TARGET ${DEFAULT_IOS_DEPL_TARGET})
        set_xcode_property(spark IPHONEOS_DEPLOYMENT_TARGET ${DEFAULT_IOS_DEPL_TARGET})
        set_xcode_property(squirrel IPHONEOS_DEPLOYMENT_TARGET ${DEFAULT_IOS_DEPL_TARGET})
        set_xcode_property(${FREETYPE_LIBRARY} IPHONEOS_DEPLOYMENT_TARGET ${DEFAULT_IOS_DEPL_TARGET})
        set_xcode_property(zlibstatic IPHONEOS_DEPLOYMENT_TARGET ${DEFAULT_IOS_DEPL_TARGET})
        set_xcode_property(TurboBadgerLib IPHONEOS_DEPLOYMENT_TARGET ${DEFAULT_IOS_DEPL_TARGET})
        set_xcode_property(webp IPHONEOS_DEPLOYMENT_TARGET ${DEFAULT_IOS_DEPL_TARGET})
    endif()
endif()

set_target_properties(libdf3d PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "df3d_pch.h")
set_target_properties(libdf3d PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
cotire(libdf3d)
