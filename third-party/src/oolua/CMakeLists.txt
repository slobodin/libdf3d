cmake_minimum_required(VERSION 2.8)

project(oolua)

set(OOLUA_HEADERS_ROOT ${PROJECT_SOURCE_DIR}/../../include/oolua)

set(oolua_HEADER_LIST
    ${OOLUA_HEADERS_ROOT}/char_arrays.h
    ${OOLUA_HEADERS_ROOT}/class_from_stack.h
    ${OOLUA_HEADERS_ROOT}/default_trait_caller.h
    ${OOLUA_HEADERS_ROOT}/dsl_va_args.h
    ${OOLUA_HEADERS_ROOT}/lua_includes.h
    ${OOLUA_HEADERS_ROOT}/lvd_types.h
    ${OOLUA_HEADERS_ROOT}/lvd_type_traits.h
    ${OOLUA_HEADERS_ROOT}/oolua.h
    ${OOLUA_HEADERS_ROOT}/oolua_boilerplate.h
    ${OOLUA_HEADERS_ROOT}/oolua_check_result.h
    ${OOLUA_HEADERS_ROOT}/oolua_chunk.h
    ${OOLUA_HEADERS_ROOT}/oolua_config.h
    ${OOLUA_HEADERS_ROOT}/oolua_dsl.h
    ${OOLUA_HEADERS_ROOT}/oolua_dsl_export.h
    ${OOLUA_HEADERS_ROOT}/oolua_error.h
    ${OOLUA_HEADERS_ROOT}/oolua_exception.h
    ${OOLUA_HEADERS_ROOT}/oolua_function.h
    ${OOLUA_HEADERS_ROOT}/oolua_helpers.h
    ${OOLUA_HEADERS_ROOT}/oolua_open.h
    ${OOLUA_HEADERS_ROOT}/oolua_pull.h
    ${OOLUA_HEADERS_ROOT}/oolua_push.h
    ${OOLUA_HEADERS_ROOT}/oolua_ref.h
    ${OOLUA_HEADERS_ROOT}/oolua_registration.h
    ${OOLUA_HEADERS_ROOT}/oolua_registration_fwd.h
    ${OOLUA_HEADERS_ROOT}/oolua_script.h
    ${OOLUA_HEADERS_ROOT}/oolua_stack.h
    ${OOLUA_HEADERS_ROOT}/oolua_stack_dump.h
    ${OOLUA_HEADERS_ROOT}/oolua_stack_fwd.h
    ${OOLUA_HEADERS_ROOT}/oolua_string.h
    ${OOLUA_HEADERS_ROOT}/oolua_table.h
    ${OOLUA_HEADERS_ROOT}/oolua_traits.h
    ${OOLUA_HEADERS_ROOT}/oolua_traits_fwd.h
    ${OOLUA_HEADERS_ROOT}/oolua_version.h
    ${OOLUA_HEADERS_ROOT}/platform_check.h
    ${OOLUA_HEADERS_ROOT}/proxy_base_checker.h
    ${OOLUA_HEADERS_ROOT}/proxy_caller.h
    ${OOLUA_HEADERS_ROOT}/proxy_class.h
    ${OOLUA_HEADERS_ROOT}/proxy_constructor.h
    ${OOLUA_HEADERS_ROOT}/proxy_constructor_param_tester.h
    ${OOLUA_HEADERS_ROOT}/proxy_function_dispatch.h
    ${OOLUA_HEADERS_ROOT}/proxy_function_exports.h
    ${OOLUA_HEADERS_ROOT}/proxy_member_function.h
    ${OOLUA_HEADERS_ROOT}/proxy_none_member_function.h
    ${OOLUA_HEADERS_ROOT}/proxy_operators.h
    ${OOLUA_HEADERS_ROOT}/proxy_public_member.h
    ${OOLUA_HEADERS_ROOT}/proxy_stack_helper.h
    ${OOLUA_HEADERS_ROOT}/proxy_storage.h
    ${OOLUA_HEADERS_ROOT}/proxy_tags.h
    ${OOLUA_HEADERS_ROOT}/proxy_tag_info.h
    ${OOLUA_HEADERS_ROOT}/proxy_test.h
    ${OOLUA_HEADERS_ROOT}/proxy_userdata.h
    ${OOLUA_HEADERS_ROOT}/push_pointer_internal.h
    ${OOLUA_HEADERS_ROOT}/stack_get.h
    ${OOLUA_HEADERS_ROOT}/typelist_structs.h
    ${OOLUA_HEADERS_ROOT}/type_converters.h
    ${OOLUA_HEADERS_ROOT}/type_list.h
)

set(oolua_SRC_LIST
    ${PROJECT_SOURCE_DIR}/class_from_stack.cpp
    ${PROJECT_SOURCE_DIR}/oolua.cpp
    ${PROJECT_SOURCE_DIR}/oolua_check_result.cpp
    ${PROJECT_SOURCE_DIR}/oolua_chunk.cpp
    ${PROJECT_SOURCE_DIR}/oolua_error.cpp
    ${PROJECT_SOURCE_DIR}/oolua_exception.cpp
    ${PROJECT_SOURCE_DIR}/oolua_function.cpp
    ${PROJECT_SOURCE_DIR}/oolua_helpers.cpp
    ${PROJECT_SOURCE_DIR}/oolua_open.cpp
    ${PROJECT_SOURCE_DIR}/oolua_pull.cpp
    ${PROJECT_SOURCE_DIR}/oolua_push.cpp
    ${PROJECT_SOURCE_DIR}/oolua_ref.cpp
    ${PROJECT_SOURCE_DIR}/oolua_registration.cpp
    ${PROJECT_SOURCE_DIR}/oolua_script.cpp
    ${PROJECT_SOURCE_DIR}/oolua_stack_dump.cpp
    ${PROJECT_SOURCE_DIR}/oolua_string.cpp
    ${PROJECT_SOURCE_DIR}/oolua_table.cpp
    ${PROJECT_SOURCE_DIR}/proxy_storage.cpp
    ${PROJECT_SOURCE_DIR}/push_pointer_internal.cpp
    ${PROJECT_SOURCE_DIR}/stack_get.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/../../include)
include_directories(${PROJECT_SOURCE_DIR}/../../include/oolua)

if (WINDOWS)
    add_definitions(-DWIN32 -DDF3D_WINDOWS -DPLATFORM_CHECKED -DWINDOWS_BUILD)
endif()

if (WINDOWS_PHONE)
    add_definitions(-DDF3D_WINDOWS_PHONE)
endif()

add_library(oolua STATIC ${oolua_SRC_LIST} ${oolua_HEADER_LIST})
