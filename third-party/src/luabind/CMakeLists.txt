cmake_minimum_required(VERSION 2.8)

project(luabind)

set(LUABIND_HEADERS_ROOT ${PROJECT_SOURCE_DIR}/../../include/luabind)

# set(luabind_HEADER_LIST
#     ${LUABIND_HEADERS_ROOT}/adopt_policy.hpp
#     ${LUABIND_HEADERS_ROOT}/back_reference.hpp
#     ${LUABIND_HEADERS_ROOT}/back_reference_fwd.hpp
#     ${LUABIND_HEADERS_ROOT}/class.hpp
#     ${LUABIND_HEADERS_ROOT}/class_info.hpp
#     ${LUABIND_HEADERS_ROOT}/config.hpp
#     ${LUABIND_HEADERS_ROOT}/container_policy.hpp
#     ${LUABIND_HEADERS_ROOT}/copy_policy.hpp
#     ${LUABIND_HEADERS_ROOT}/dependency_policy.hpp
#     ${LUABIND_HEADERS_ROOT}/discard_result_policy.hpp
#     ${LUABIND_HEADERS_ROOT}/error.hpp
#     ${LUABIND_HEADERS_ROOT}/from_stack.hpp
#     ${LUABIND_HEADERS_ROOT}/function.hpp
#     ${LUABIND_HEADERS_ROOT}/get_pointer.hpp
#     ${LUABIND_HEADERS_ROOT}/handle.hpp
#     ${LUABIND_HEADERS_ROOT}/iterator_policy.hpp
#     ${LUABIND_HEADERS_ROOT}/lua_include.hpp
#     ${LUABIND_HEADERS_ROOT}/luabind.hpp
#     ${LUABIND_HEADERS_ROOT}/nil.hpp
#     ${LUABIND_HEADERS_ROOT}/object.hpp
#     ${LUABIND_HEADERS_ROOT}/open.hpp
#     ${LUABIND_HEADERS_ROOT}/operator.hpp
#     ${LUABIND_HEADERS_ROOT}/out_value_policy.hpp
#     ${LUABIND_HEADERS_ROOT}/prefix.hpp
#     ${LUABIND_HEADERS_ROOT}/raw_policy.hpp
#     ${LUABIND_HEADERS_ROOT}/return_reference_to_policy.hpp
#     ${LUABIND_HEADERS_ROOT}/scope.hpp
#     ${LUABIND_HEADERS_ROOT}/value_wrapper.hpp
#     ${LUABIND_HEADERS_ROOT}/weak_ref.hpp
#     ${LUABIND_HEADERS_ROOT}/wrapper_base.hpp
#     ${LUABIND_HEADERS_ROOT}/yield_policy.hpp
# )

set(luabind_SRC_LIST
    ${PROJECT_SOURCE_DIR}/class.cpp
    ${PROJECT_SOURCE_DIR}/class_info.cpp
    ${PROJECT_SOURCE_DIR}/class_registry.cpp
    ${PROJECT_SOURCE_DIR}/class_rep.cpp
    ${PROJECT_SOURCE_DIR}/create_class.cpp
    ${PROJECT_SOURCE_DIR}/error.cpp
    ${PROJECT_SOURCE_DIR}/exception_handler.cpp
    ${PROJECT_SOURCE_DIR}/function.cpp
    ${PROJECT_SOURCE_DIR}/inheritance.cpp
    ${PROJECT_SOURCE_DIR}/link_compatibility.cpp
    ${PROJECT_SOURCE_DIR}/object_rep.cpp
    ${PROJECT_SOURCE_DIR}/open.cpp
    ${PROJECT_SOURCE_DIR}/pcall.cpp
    ${PROJECT_SOURCE_DIR}/scope.cpp
    ${PROJECT_SOURCE_DIR}/stack_content_by_name.cpp
    ${PROJECT_SOURCE_DIR}/weak_ref.cpp
    ${PROJECT_SOURCE_DIR}/wrapper_base.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/../../include/
                    ${Boost_INCLUDE_DIRS})

add_definitions(-DLUA_COMPAT_5_1)

if (WINDOWS)
    add_definitions(-DWIN32 -DDF3D_WINDOWS)
endif()

if (WINDOWS_PHONE)
    add_definitions(-DDF3D_WINDOWS_PHONE)
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
endif(CMAKE_COMPILER_IS_GNUCXX)

if(MSVC)
    set_source_files_properties(${luabind_SRC_LIST} PROPERTIES COMPILE_FLAGS "/wd\"4459\" /wd\"4456\"")
endif(MSVC)

add_library(luabind STATIC ${luabind_SRC_LIST})
