cmake_minimum_required(VERSION 2.8)

project(libspark)

set(LIBSPARK_HEADERS_ROOT ${PROJECT_SOURCE_DIR}/../../include/spark)

set(libspark_HEADER_LIST
    ${LIBSPARK_HEADERS_ROOT}/SPK.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_ArrayBuffer.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Buffer.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_BufferHandler.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_DEF.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Emitter.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Factory.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Group.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Interpolator.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Model.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Modifier.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Particle.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Pool.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Registerable.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_RegWrapper.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Renderer.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_System.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Transformable.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Vector3D.h
    ${LIBSPARK_HEADERS_ROOT}/Core/SPK_Zone.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Emitters/SPK_NormalEmitter.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Emitters/SPK_RandomEmitter.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Emitters/SPK_SphericEmitter.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Emitters/SPK_StaticEmitter.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Emitters/SPK_StraightEmitter.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_Collision.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_Destroyer.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_LinearForce.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_ModifierGroup.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_Obstacle.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_PointMass.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_Rotator.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Modifiers/SPK_Vortex.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Renderers/SPK_LineRendererInterface.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Renderers/SPK_Oriented2DRendererInterface.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Renderers/SPK_Oriented3DRendererInterface.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Renderers/SPK_PointRendererInterface.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Renderers/SPK_QuadRendererInterface.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Zones/SPK_AABox.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Zones/SPK_Cylinder.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Zones/SPK_Line.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Zones/SPK_Plane.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Zones/SPK_Point.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Zones/SPK_Ring.h
    ${LIBSPARK_HEADERS_ROOT}/Extensions/Zones/SPK_Sphere.h
)

set(libspark_SRC_LIST
    ${PROJECT_SOURCE_DIR}/SPK_All.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/../../include/spark)

if(MSVC)
    add_definitions(-DWIN32)

    set_source_files_properties(${libspark_SRC_LIST} PROPERTIES COMPILE_FLAGS "/wd\"4456\" /wd\"4838\" /wd\"4457\" /wd\"4458\"")
endif(MSVC)

add_library(libspark STATIC ${libspark_SRC_LIST} ${libspark_HEADER_LIST})