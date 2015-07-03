cmake_minimum_required(VERSION 2.8)

project(spark)

include(${PROJECT_SOURCE_DIR}/../../../cmake/common.cmake)

set(SPARK_HEADERS_ROOT ${PROJECT_SOURCE_DIR}/../include)

set(spark_HEADER_LIST
    ${SPARK_HEADERS_ROOT}/SPARK.h
    ${SPARK_HEADERS_ROOT}/SPARK_Core.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Action.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_ArrayData.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Attributes.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_ClassDescription.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Color.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_ConnectionIterators.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Connections.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Controller.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Controls.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_DataHandler.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_DataSet.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_DEF.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_DescriptionDefines.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Emitter.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Enum.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Factory.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Getters.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Group.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Interpolator.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Iterator.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Logger.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_MemoryTracer.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Meta.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Modifier.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Object.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Octree.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Particle.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Reference.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_RenderBuffer.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Renderer.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Setters.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_StaticDescription.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_System.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Traits.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Transform.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Transformable.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_TypeOperations.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Types.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Vector3D.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_Zone.h
    ${SPARK_HEADERS_ROOT}/Core/SPK_ZonedModifier.h
    ${SPARK_HEADERS_ROOT}/Core/IO/SPK_IO_Buffer.h
    ${SPARK_HEADERS_ROOT}/Core/IO/SPK_IO_Context.h
    ${SPARK_HEADERS_ROOT}/Core/IO/SPK_IO_Deserializer.h
    ${SPARK_HEADERS_ROOT}/Core/IO/SPK_IO_Loader.h
    ${SPARK_HEADERS_ROOT}/Core/IO/SPK_IO_Manager.h
    ${SPARK_HEADERS_ROOT}/Core/IO/SPK_IO_Saver.h
    ${SPARK_HEADERS_ROOT}/Core/IO/SPK_IO_Serializer.h
    ${SPARK_HEADERS_ROOT}/Extensions/Actions/SPK_ActionSet.h
    ${SPARK_HEADERS_ROOT}/Extensions/Actions/SPK_SpawnParticlesAction.h
    ${SPARK_HEADERS_ROOT}/Extensions/Emitters/SPK_NormalEmitter.h
    ${SPARK_HEADERS_ROOT}/Extensions/Emitters/SPK_RandomEmitter.h
    ${SPARK_HEADERS_ROOT}/Extensions/Emitters/SPK_SphericEmitter.h
    ${SPARK_HEADERS_ROOT}/Extensions/Emitters/SPK_StaticEmitter.h
    ${SPARK_HEADERS_ROOT}/Extensions/Emitters/SPK_StraightEmitter.h
    ${SPARK_HEADERS_ROOT}/Extensions/Interpolators/SPK_DefaultInitializer.h
    ${SPARK_HEADERS_ROOT}/Extensions/Interpolators/SPK_GraphInterpolator.h
    ${SPARK_HEADERS_ROOT}/Extensions/Interpolators/SPK_RandomInitializer.h
    ${SPARK_HEADERS_ROOT}/Extensions/Interpolators/SPK_RandomInterpolator.h
    ${SPARK_HEADERS_ROOT}/Extensions/Interpolators/SPK_SimpleInterpolator.h
    ${SPARK_HEADERS_ROOT}/Extensions/IOConverters/SPK_IO_SPKCommon.h
    ${SPARK_HEADERS_ROOT}/Extensions/IOConverters/SPK_IO_SPKLoader.h
    ${SPARK_HEADERS_ROOT}/Extensions/IOConverters/SPK_IO_SPKSaver.h
    ${SPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_BasicModifiers.h
    ${SPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_Collider.h
    ${SPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_Destroyer.h
    ${SPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_EmitterAttacher.h
    ${SPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_LinearForce.h
    ${SPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_Obstacle.h
    ${SPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_PointMass.h
    ${SPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_RandomForce.h
    ${SPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_Rotator.h
    ${SPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_Vortex.h
    ${SPARK_HEADERS_ROOT}/Extensions/Renderers/SPK_LineRenderBehavior.h
    ${SPARK_HEADERS_ROOT}/Extensions/Renderers/SPK_Oriented3DRenderBehavior.h
    ${SPARK_HEADERS_ROOT}/Extensions/Renderers/SPK_PointRenderBehavior.h
    ${SPARK_HEADERS_ROOT}/Extensions/Renderers/SPK_QuadRenderBehavior.h
    ${SPARK_HEADERS_ROOT}/Extensions/Zones/SPK_Box.h
    ${SPARK_HEADERS_ROOT}/Extensions/Zones/SPK_Cylinder.h
    ${SPARK_HEADERS_ROOT}/Extensions/Zones/SPK_Plane.h
    ${SPARK_HEADERS_ROOT}/Extensions/Zones/SPK_Point.h
    ${SPARK_HEADERS_ROOT}/Extensions/Zones/SPK_Ring.h
    ${SPARK_HEADERS_ROOT}/Extensions/Zones/SPK_Sphere.h
)

set(spark_SRC_LIST
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
    # ${PROJECT_SOURCE_DIR}/Core/IO/SPK_IO_Buffer.cpp
    # ${PROJECT_SOURCE_DIR}/Core/IO/SPK_IO_Manager.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Actions/SPK_ActionSet.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Actions/SPK_SpawnParticlesAction.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Emitters/SPK_NormalEmitter.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Emitters/SPK_RandomEmitter.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Emitters/SPK_SphericEmitter.cpp
    ${PROJECT_SOURCE_DIR}/Extensions/Emitters/SPK_StraightEmitter.cpp
    # ${PROJECT_SOURCE_DIR}/Extensions/IOConverters/SPK_IO_SPKLoader.cpp
    # ${PROJECT_SOURCE_DIR}/Extensions/IOConverters/SPK_IO_SPKSaver.cpp
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

include_directories(${PROJECT_SOURCE_DIR}/../include)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

if(MSVC)
    set_source_files_properties(${spark_SRC_LIST} PROPERTIES COMPILE_FLAGS "/wd\"4456\" /wd\"4838\" /wd\"4457\" /wd\"4458\"")
endif(MSVC)

if (WINDOWS_PHONE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif()

add_library(spark STATIC ${spark_SRC_LIST} ${spark_HEADER_LIST})
