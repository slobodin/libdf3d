cmake_minimum_required(VERSION 2.8)

project(libspark)

set(LIBSPARK_HEADERS_ROOT ${PROJECT_SOURCE_DIR}/../../include/spark)

set(libspark_HEADER_LIST
    ${LIBSPARK_HEADERS_ROOT}/SPARK.h
    ${LIBSPARK_HEADERS_ROOT}/SPARK_Core.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Action.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_ArrayData.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Attributes.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_ClassDescription.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Color.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_ConnectionIterators.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Connections.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Controller.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Controls.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_DataHandler.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_DataSet.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_DEF.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_DescriptionDefines.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Emitter.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Enum.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Factory.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Getters.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Group.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Interpolator.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Iterator.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Logger.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_MemoryTracer.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Meta.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Modifier.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Object.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Octree.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Particle.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Reference.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_RenderBuffer.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Renderer.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Setters.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_StaticDescription.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_System.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Traits.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Transform.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Transformable.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_TypeOperations.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Types.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Vector3D.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Zone.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_ZonedModifier.h
    ${LIBSPARK_HEADERS_ROOT}/Core/IO/SPK_IO_Buffer.h
    ${LIBSPARK_HEADERS_ROOT}/Core/IO/SPK_IO_Context.h
    ${LIBSPARK_HEADERS_ROOT}/Core/IO/SPK_IO_Deserializer.h
    ${LIBSPARK_HEADERS_ROOT}/Core/IO/SPK_IO_Loader.h
    ${LIBSPARK_HEADERS_ROOT}/Core/IO/SPK_IO_Manager.h
    ${LIBSPARK_HEADERS_ROOT}/Core/IO/SPK_IO_Saver.h
    ${LIBSPARK_HEADERS_ROOT}/Core/IO/SPK_IO_Serializer.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Actions/SPK_ActionSet.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Actions/SPK_SpawnParticlesAction.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Emitters/SPK_NormalEmitter.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Emitters/SPK_RandomEmitter.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Emitters/SPK_SphericEmitter.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Emitters/SPK_StaticEmitter.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Emitters/SPK_StraightEmitter.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Interpolators/SPK_DefaultInitializer.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Interpolators/SPK_GraphInterpolator.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Interpolators/SPK_RandomInitializer.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Interpolators/SPK_RandomInterpolator.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Interpolators/SPK_SimpleInterpolator.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/IOConverters/SPK_IO_SPKCommon.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/IOConverters/SPK_IO_SPKLoader.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/IOConverters/SPK_IO_SPKSaver.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_BasicModifiers.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_Collider.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_Destroyer.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_EmitterAttacher.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_LinearForce.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_Obstacle.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_PointMass.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_RandomForce.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_Rotator.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_Vortex.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Renderers/SPK_LineRenderBehavior.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Renderers/SPK_Oriented3DRenderBehavior.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Renderers/SPK_PointRenderBehavior.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Renderers/SPK_QuadRenderBehavior.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Zones/SPK_Box.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Zones/SPK_Cylinder.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Zones/SPK_Plane.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Zones/SPK_Point.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Zones/SPK_Ring.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Zones/SPK_Sphere.h
)

set(libspark_SRC_LIST
    ${PROJECT_SOURCE_DIR}/Core/SPK_Connections.cpp
    ${PROJECT_SOURCE_DIR}/Core/SPK_DataSet.cpp
    ${PROJECT_SOURCE_DIR}/Core/SPK_DEF.cpp
    ${PROJECT_SOURCE_DIR}/Core/SPK_Emitter.cpp
    ${PROJECT_SOURCE_DIR}/Core/SPK_Factory.cpp
    ${PROJECT_SOURCE_DIR}/Core/SPK_Group.cpp
    ${PROJECT_SOURCE_DIR}/Core/SPK_Logger.cpp
    ${PROJECT_SOURCE_DIR}/Core/SPK_MemoryTracer.cpp
    ${PROJECT_SOURCE_DIR}/Core/SPK_Object.cpp
    ${PROJECT_SOURCE_DIR}/Core/SPK_Octree.cpp
    ${PROJECT_SOURCE_DIR}/Core/SPK_Renderer.cpp
    ${PROJECT_SOURCE_DIR}/Core/SPK_System.cpp
    ${PROJECT_SOURCE_DIR}/Core/SPK_Transform.cpp
    ${PROJECT_SOURCE_DIR}/Core/SPK_Transformable.cpp
    ${PROJECT_SOURCE_DIR}/Core/SPK_Types.cpp
    ${PROJECT_SOURCE_DIR}/Core/SPK_Zone.cpp
    ${PROJECT_SOURCE_DIR}/Core/SPK_ZonedModifier.cpp
    ${PROJECT_SOURCE_DIR}/Core/IO/SPK_IO_Buffer.cpp
    ${PROJECT_SOURCE_DIR}/Core/IO/SPK_IO_Manager.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Actions/SPK_ActionSet.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Actions/SPK_SpawnParticlesAction.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Emitters/SPK_NormalEmitter.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Emitters/SPK_RandomEmitter.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Emitters/SPK_SphericEmitter.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Emitters/SPK_StraightEmitter.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/IOConverters/SPK_IO_SPKLoader.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/IOConverters/SPK_IO_SPKSaver.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Modifiers/SPK_BasicModifiers.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Modifiers/SPK_Collider.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Modifiers/SPK_EmitterAttacher.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Modifiers/SPK_LinearForce.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Modifiers/SPK_Obstacle.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Modifiers/SPK_PointMass.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Modifiers/SPK_RandomForce.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Modifiers/SPK_Rotator.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Modifiers/SPK_Vortex.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Renderers/SPK_Oriented3DRenderBehavior.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Renderers/SPK_QuadRenderBehavior.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Zones/SPK_Box.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Zones/SPK_Cylinder.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Zones/SPK_Plane.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Zones/SPK_Ring.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Zones/SPK_Sphere.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/../../include/spark)

if(MSVC)
    set_source_files_properties(${libspark_SRC_LIST} PROPERTIES COMPILE_FLAGS "/wd\"4456\" /wd\"4838\" /wd\"4457\" /wd\"4458\"")
endif(MSVC)

if (WINDOWS)
    add_definitions(-DWIN32 -DDF3D_WINDOWS)
endif()

if (WINDOWS_PHONE)
    add_definitions(-DDF3D_WINDOWS_PHONE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif()

add_library(libspark STATIC ${libspark_SRC_LIST} ${libspark_HEADER_LIST})