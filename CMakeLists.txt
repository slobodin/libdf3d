cmake_minimum_required(VERSION 3.1)

project(libdf3d)

if (NOT DEFINED DF3D_PLATFORM)
    message(FATAL_ERROR "Please, include libdf3d.cmake")
else()
    message(STATUS "DF3D_PLATFORM is " ${DF3D_PLATFORM})
endif()

set(DF3D_ROOT ${PROJECT_SOURCE_DIR})

include(${PROJECT_SOURCE_DIR}/cmake/cotire.cmake)

if (WINDOWS AND NOT CMAKE_PREFIX_PATH)
    set (CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/third-party/prebuilt_win32" CACHE PATH "third-party")
endif()

# if (NOT ANDROID AND NOT IOS)
    option(DF3D_BUILD_SHARED_LIB "Build shared df3d library" OFF)
# endif()

set(libdf3d_HEADER_LIST
    ${PROJECT_SOURCE_DIR}/df3d/Common.h
    ${PROJECT_SOURCE_DIR}/df3d/df3d.h
    ${PROJECT_SOURCE_DIR}/df3d/df3d_config.h
    ${PROJECT_SOURCE_DIR}/df3d/df3d_pch.h
    ${PROJECT_SOURCE_DIR}/df3d/libdf3d_dll.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/ConfigVariable.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/EngineController.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/EngineCVars.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/EngineInitParams.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/TimeManager.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/2d/Sprite2DComponentProcessor.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/3d/Camera.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/3d/Light.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/3d/SceneGraphComponentProcessor.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/3d/StaticMeshComponentProcessor.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/audio/AudioWorld.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/audio/AudioManager.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/audio/OpenALCommon.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/gui/GuiManager.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/gui/TBInterface.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/input/InputEvents.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/input/InputManager.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/io/FileSystemHelpers.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/io/Storage.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/particlesys/ParticleSystemComponentProcessor.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/particlesys/ParticleSystemUtils.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/particlesys/SparkCommon.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/particlesys/SparkLineTrailRenderer.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/particlesys/SparkQuadRenderer.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/physics/btGImpactConvexDecompositionShape.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/physics/BulletInterface.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/physics/PhysicsComponentCreationParams.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/physics/PhysicsComponentProcessor.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/physics/PhysicsHelpers.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/GpuProgramSharedState.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/IRenderBackend.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/Material.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/MeshUtils.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/RenderCommon.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/RenderManager.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/RenderOperation.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/RenderQueue.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/Vertex.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/Viewport.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/gl/GLSLPreprocess.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/gl/RenderBackendGL.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/gl/embed_glsl/ambient_frag.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/gl/embed_glsl/ambient_vert.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/gl/embed_glsl/colored_frag.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/gl/embed_glsl/colored_vert.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/AudioResource.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/EntityResource.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/GpuProgramResource.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/IResourceHolder.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/MaterialResource.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/MeshResource.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/ParticleSystemResource.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/ResourceDataSource.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/ResourceFileSystem.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/ResourceManager.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/TextureResource.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/loaders/AudioLoader_ogg.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/loaders/AudioLoader_wav.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/loaders/MeshLoader_dfmesh.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/loaders/MeshLoader_obj.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/loaders/TextureLoader_pvrtc.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/loaders/TextureLoader_stbi.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/loaders/TextureLoader_webp.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/script/ScriptBindings.h
    ${PROJECT_SOURCE_DIR}/df3d/engine/script/ScriptManager.h
    ${PROJECT_SOURCE_DIR}/df3d/game/ComponentDataHolder.h
    ${PROJECT_SOURCE_DIR}/df3d/game/Entity.h
    ${PROJECT_SOURCE_DIR}/df3d/game/EntityComponentLoader.h
    ${PROJECT_SOURCE_DIR}/df3d/game/EntityComponentProcessor.h
    ${PROJECT_SOURCE_DIR}/df3d/game/FPSCamera.h
    ${PROJECT_SOURCE_DIR}/df3d/game/TagComponentProcessor.h
    ${PROJECT_SOURCE_DIR}/df3d/game/World.h
    ${PROJECT_SOURCE_DIR}/df3d/game/WorldRenderingParams.h
    ${PROJECT_SOURCE_DIR}/df3d/game/WorldSize.h
    ${PROJECT_SOURCE_DIR}/df3d/game/impl/EntityLoader.h
    ${PROJECT_SOURCE_DIR}/df3d/game/impl/MeshComponentLoader.h
    ${PROJECT_SOURCE_DIR}/df3d/game/impl/PhysicsComponentLoader.h
    ${PROJECT_SOURCE_DIR}/df3d/game/impl/SceneGraphComponentLoader.h
    ${PROJECT_SOURCE_DIR}/df3d/game/impl/Sprite2DComponentLoader.h
    ${PROJECT_SOURCE_DIR}/df3d/game/impl/VfxComponentLoader.h
    ${PROJECT_SOURCE_DIR}/df3d/game/impl/WorldLoader.h
    ${PROJECT_SOURCE_DIR}/df3d/lib/Any.h
    ${PROJECT_SOURCE_DIR}/df3d/lib/Dict.h
    ${PROJECT_SOURCE_DIR}/df3d/lib/Handles.h
    ${PROJECT_SOURCE_DIR}/df3d/lib/JsonUtils.h
    ${PROJECT_SOURCE_DIR}/df3d/lib/Log.h
    ${PROJECT_SOURCE_DIR}/df3d/lib/NonCopyable.h
    ${PROJECT_SOURCE_DIR}/df3d/lib/ThreadPool.h
    ${PROJECT_SOURCE_DIR}/df3d/lib/Utils.h
    ${PROJECT_SOURCE_DIR}/df3d/lib/assert/Assert.h
    ${PROJECT_SOURCE_DIR}/df3d/lib/containers/ConcurrentQueue.h
    ${PROJECT_SOURCE_DIR}/df3d/lib/containers/PodArray.h
    ${PROJECT_SOURCE_DIR}/df3d/lib/math/AABB.h
    ${PROJECT_SOURCE_DIR}/df3d/lib/math/BoundingSphere.h
    ${PROJECT_SOURCE_DIR}/df3d/lib/math/BoundingVolume.h
    ${PROJECT_SOURCE_DIR}/df3d/lib/math/ConvexHull.h
    ${PROJECT_SOURCE_DIR}/df3d/lib/math/Frustum.h
    ${PROJECT_SOURCE_DIR}/df3d/lib/math/MathUtils.h
    ${PROJECT_SOURCE_DIR}/df3d/lib/math/OBB.h
    ${PROJECT_SOURCE_DIR}/df3d/lib/memory/Allocator.h
    ${PROJECT_SOURCE_DIR}/df3d/lib/memory/MallocAllocator.h
    ${PROJECT_SOURCE_DIR}/df3d/lib/memory/PoolAllocator.h
    ${PROJECT_SOURCE_DIR}/df3d/lib/os/PlatformFile.h
    ${PROJECT_SOURCE_DIR}/df3d/lib/os/PlatformStorage.h
    ${PROJECT_SOURCE_DIR}/df3d/lib/os/PlatformUtils.h
    ${PROJECT_SOURCE_DIR}/df3d/platform/AppDelegate.h
    ${PROJECT_SOURCE_DIR}/df3d/platform/LocalNotification.h
    ${PROJECT_SOURCE_DIR}/df3d/platform/InAppPurchase.h
)

set(libdf3d_SRC_LIST
    ${PROJECT_SOURCE_DIR}/df3d/Common.cpp
    ${PROJECT_SOURCE_DIR}/df3d/df3d_pch.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/EngineController.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/EngineCVars.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/TimeManager.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/2d/Sprite2DComponentProcessor.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/3d/Camera.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/3d/Light.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/3d/SceneGraphComponentProcessor.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/3d/StaticMeshComponentProcessor.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/audio/AudioWorld.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/audio/AudioManager.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/audio/OpenALCommon.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/gui/GuiManager.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/gui/TBInterface.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/input/InputEvents.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/input/InputManager.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/io/FileSystemHelpers.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/io/Storage.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/particlesys/ParticleSystemComponentProcessor.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/particlesys/ParticleSystemUtils.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/particlesys/SparkCommon.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/particlesys/SparkLineTrailRenderer.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/particlesys/SparkQuadRenderer.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/physics/btGImpactConvexDecompositionShape.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/physics/BulletInterface.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/physics/PhysicsComponentCreationParams.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/physics/PhysicsComponentProcessor.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/GpuProgramSharedState.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/Material.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/MeshUtils.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/RenderCommon.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/RenderManager.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/RenderOperation.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/RenderQueue.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/Vertex.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/Viewport.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/gl/GLSLPreprocess.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/render/gl/RenderBackendGL.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/AudioResource.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/EntityResource.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/GpuProgramResource.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/MaterialResource.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/MeshResource.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/ParticleSystemResource.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/ResourceDataSource.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/ResourceFileSystem.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/ResourceManager.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/TextureResource.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/loaders/AudioLoader_ogg.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/loaders/AudioLoader_wav.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/loaders/MeshLoader_dfmesh.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/loaders/MeshLoader_obj.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/loaders/TextureLoader_pvrtc.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/loaders/TextureLoader_stbi.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/resources/loaders/TextureLoader_webp.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/script/ScriptBindings.cpp
    ${PROJECT_SOURCE_DIR}/df3d/engine/script/ScriptManager.cpp
    ${PROJECT_SOURCE_DIR}/df3d/game/FPSCamera.cpp
    ${PROJECT_SOURCE_DIR}/df3d/game/TagComponentProcessor.cpp
    ${PROJECT_SOURCE_DIR}/df3d/game/World.cpp
    ${PROJECT_SOURCE_DIR}/df3d/game/WorldRenderingParams.cpp
    ${PROJECT_SOURCE_DIR}/df3d/game/impl/EntityLoader.cpp
    ${PROJECT_SOURCE_DIR}/df3d/game/impl/WorldLoader.cpp
    ${PROJECT_SOURCE_DIR}/df3d/lib/Dict.cpp
    ${PROJECT_SOURCE_DIR}/df3d/lib/Handles.cpp
    ${PROJECT_SOURCE_DIR}/df3d/lib/JsonUtils.cpp
    ${PROJECT_SOURCE_DIR}/df3d/lib/Log.cpp
    ${PROJECT_SOURCE_DIR}/df3d/lib/ThreadPool.cpp
    ${PROJECT_SOURCE_DIR}/df3d/lib/Utils.cpp
    ${PROJECT_SOURCE_DIR}/df3d/lib/assert/Assert.cpp
    ${PROJECT_SOURCE_DIR}/df3d/lib/memory/MallocAllocator.cpp
    ${PROJECT_SOURCE_DIR}/df3d/lib/memory/PoolAllocator.cpp
    ${PROJECT_SOURCE_DIR}/df3d/lib/math/AABB.cpp
    ${PROJECT_SOURCE_DIR}/df3d/lib/math/BoundingSphere.cpp
    ${PROJECT_SOURCE_DIR}/df3d/lib/math/ConvexHull.cpp
    ${PROJECT_SOURCE_DIR}/df3d/lib/math/Frustum.cpp
    ${PROJECT_SOURCE_DIR}/df3d/lib/math/MathUtils.cpp
    ${PROJECT_SOURCE_DIR}/df3d/lib/math/OBB.cpp
    ${PROJECT_SOURCE_DIR}/df3d/platform/AppDelegate.cpp
)

if (NOT (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    add_definitions(-DSCRAT_NO_ERROR_CHECKING)
    add_definitions(-DBT_NO_PROFILE)
endif()

set(third_party_SRC_FILES
    ${PROJECT_SOURCE_DIR}/third-party/json/jsoncpp.cpp
    ${PROJECT_SOURCE_DIR}/third-party/mikktspace/mikktspace.c
)

if (WINDOWS)
    list(APPEND libdf3d_HEADER_LIST
        ${PROJECT_SOURCE_DIR}/df3d/platform/windows/CrashHandler.h
    )
    list(APPEND libdf3d_SRC_LIST
        ${PROJECT_SOURCE_DIR}/df3d/platform/windows/CrashHandler.cpp
        ${PROJECT_SOURCE_DIR}/df3d/platform/windows/InAppPurchase.cpp
    )
endif()

if (WINDOWS_PHONE)
    include_directories(${PROJECT_SOURCE_DIR}/third-party/include/angle)

    list(APPEND libdf3d_HEADER_LIST
        ${PROJECT_SOURCE_DIR}/df3d/platform/windows_phone/WindowsPhoneApplication.h
    )
    list(APPEND libdf3d_SRC_LIST
        ${PROJECT_SOURCE_DIR}/df3d/platform/windows_phone/Storage.cpp
        ${PROJECT_SOURCE_DIR}/df3d/platform/windows_phone/WindowsPhoneApplication.cpp
    )
endif()

if (ANDROID)
    list(APPEND libdf3d_SRC_LIST
        ${PROJECT_SOURCE_DIR}/df3d/lib/os/PlatformFile_android.cpp
        ${PROJECT_SOURCE_DIR}/df3d/lib/os/PlatformStorage_android.cpp
        ${PROJECT_SOURCE_DIR}/df3d/lib/os/PlatformUtils_android.cpp

        ${PROJECT_SOURCE_DIR}/df3d/platform/android/AndroidBridge.cpp
        ${PROJECT_SOURCE_DIR}/df3d/platform/android/JNIHelpers.cpp
        ${PROJECT_SOURCE_DIR}/df3d/platform/android/LocalNotification.cpp
    )
endif()

if (IOS)
    list(APPEND libdf3d_HEADER_LIST
        ${PROJECT_SOURCE_DIR}/df3d/platform/ios/AppDelegate.h
        ${PROJECT_SOURCE_DIR}/df3d/platform/ios/GameViewController.h
    )
    list(APPEND libdf3d_SRC_LIST
        ${PROJECT_SOURCE_DIR}/df3d/lib/os/PlatformFile_ios.mm
        ${PROJECT_SOURCE_DIR}/df3d/lib/os/PlatformStorage_ios.mm
        ${PROJECT_SOURCE_DIR}/df3d/lib/os/PlatformUtils_ios.mm

        ${PROJECT_SOURCE_DIR}/df3d/platform/ios/AppDelegate.mm
        ${PROJECT_SOURCE_DIR}/df3d/platform/ios/GameViewController.mm
        ${PROJECT_SOURCE_DIR}/df3d/platform/ios/LocalNotification.mm
        ${PROJECT_SOURCE_DIR}/df3d/platform/ios/InAppPurchase.mm
    )

    set_source_files_properties("${PROJECT_SOURCE_DIR}/df3d/engine/render/gl/RenderBackendGL.cpp" PROPERTIES COMPILE_FLAGS "-x objective-c++")
endif()

if (DF3D_DESKTOP)
    list(APPEND libdf3d_HEADER_LIST
        ${PROJECT_SOURCE_DIR}/df3d/platform/desktop_common/glfwApplication.h
        ${PROJECT_SOURCE_DIR}/df3d/platform/desktop_common/glfwKeyCodes.h
    )
    list(APPEND libdf3d_SRC_LIST
        ${PROJECT_SOURCE_DIR}/df3d/lib/os/PlatformStorage_desktop.cpp
        ${PROJECT_SOURCE_DIR}/df3d/lib/os/PlatformUtils_desktop.cpp
        ${PROJECT_SOURCE_DIR}/df3d/lib/os/PlatformFile_desktop.cpp

        ${PROJECT_SOURCE_DIR}/df3d/platform/desktop_common/glfwApplication.cpp
        ${PROJECT_SOURCE_DIR}/df3d/platform/desktop_common/LocalNotification.cpp
        ${PROJECT_SOURCE_DIR}/df3d/platform/desktop_common/glfwKeyCodes.cpp
    )
endif()

if (DF3D_DESKTOP)
    find_package(GLEW REQUIRED)
    find_package(OpenGL REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
    include_directories(${OPENGL_INCLUDE_DIR})
    include_directories(${PROJECT_SOURCE_DIR}/third-party/glfw/include)
endif()

if(IOS OR MACOSX)
    find_package(OpenAL REQUIRED)
    SET(DF3D_OPENAL_LIBNAME ${OPENAL_LIBRARY})
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

if (NOT IOS AND NOT MACOSX)
    include_directories(${PROJECT_SOURCE_DIR}/third-party/openal-soft/include)
else()
    include_directories(${OPENAL_INCLUDE_DIR})
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd\"4251\" /wd\"4457\" /wd\"4458\" /wd\"4138\"")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)    #  -DGLEW_STATIC
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

set_target_properties(libdf3d PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "$(PROJECT_SOURCE_DIR)/df3d/df3d_pch.h")
set_target_properties(libdf3d PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
cotire(libdf3d)

if (DF3D_DESKTOP)
    add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/glfw)
endif()
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/bullet)
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/ogg)
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/vorbis)
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

OPTION(ALSOFT_DLOPEN OFF)
OPTION(ALSOFT_WERROR OFF)
OPTION(ALSOFT_UTILS OFF)
OPTION(ALSOFT_NO_CONFIG_UTIL OFF)
OPTION(ALSOFT_EXAMPLES OFF)
OPTION(ALSOFT_TESTS OFF)
OPTION(ALSOFT_CONFIG OFF)
OPTION(ALSOFT_HRTF_DEFS OFF)
OPTION(ALSOFT_INSTALL OFF)
OPTION(ALSOFT_AMBDEC_PRESETS OFF)

if (NOT IOS AND NOT MACOSX)
    add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/openal-soft)
    if(NOT WIN32)
        SET(DF3D_OPENAL_LIBNAME openal)
    else()
        SET(DF3D_OPENAL_LIBNAME OpenAL32)
    endif()
endif()


# TODO: if build tools
if (DF3D_DESKTOP)
    add_subdirectory(${PROJECT_SOURCE_DIR}/tools/obj_to_dfmesh)
    add_subdirectory(${PROJECT_SOURCE_DIR}/tools/glsl_preprocess)
endif()

target_link_libraries(libdf3d
    LinearMath
    BulletCollision
    BulletDynamics
    ConvexDecomposition
    ogg
    spark
    vorbis
    squirrel
    ${FREETYPE_LIBRARY}
    zlibstatic
    TurboBadgerLib
    webp)

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
