set_property(DIRECTORY . PROPERTY FOLDER "LibUbox")

# 包含 CMake 构建体系的入口脚本
include(GlobalCommon)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckFunctionExists)

OPTION(BUILD_LUA "build Lua plugin" ON)
OPTION(BUILD_EXAMPLES "build examples" ON)

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(JSONC json-c)

IF(JSONC_FOUND)
        ADD_DEFINITIONS(-DJSONC)
        INCLUDE_DIRECTORIES(${JSONC_INCLUDE_DIRS})
ENDIF()

# 设定编译源文件时的头文件搜索路径
set(LibUbox_PRIVATE_INCLUDE_DIRECTORIES
        "${CMAKE_BINARY_DIR}"
        "${LIBUBOX_DIR}"
        "${LIBUBOX_DIR}/include"
        "${LibUbox_DERIVED_SOURCES_DIR}"
        "${FORWARDING_HEADERS_DIR}"
)

# 列出所有参与编译的源文件
list(APPEND LibUbox_SOURCES
        ${LIBUBOX_DIR}/lib/avl/avl-cmp.c
        ${LIBUBOX_DIR}/lib/avl/avl.c

        ${LIBUBOX_DIR}/lib/blob/blob.c
        #${LIBUBOX_DIR}/lib/blob/blobmsg_json.c
        ${LIBUBOX_DIR}/lib/blob/blobmsg.c

        ${LIBUBOX_DIR}/lib/crypto/base64.c
        ${LIBUBOX_DIR}/lib/crypto/md5.c

        ${LIBUBOX_DIR}/lib/kvlist/kvlist.c

        ${LIBUBOX_DIR}/lib/runqueue/runqueue.c

        ${LIBUBOX_DIR}/lib/safe_list/safe_list.c

        ${LIBUBOX_DIR}/lib/ulog/ulog.c

        ${LIBUBOX_DIR}/lib/uloop/uloop.c

        ${LIBUBOX_DIR}/lib/usock/usock.c

        ${LIBUBOX_DIR}/lib/ustream/ustream-fd.c
        ${LIBUBOX_DIR}/lib/ustream/ustream.c

        ${LIBUBOX_DIR}/lib/util/utils.c

        ${LIBUBOX_DIR}/lib/vlist/vlist.c
)

set(LibUbox_LIBRARIES)
set(LibUbox_INTERFACE_LIBRARIES LibUbox)
set(LibUbox_INTERFACE_INCLUDE_DIRECTORIES ${LibUbox_PRIVATE_FRAMEWORK_HEADERS_DIR})

# 使用全局宏声明一个框架（函数库）
LIBUBOX_FRAMEWORK_DECLARE(LibUbox)
LIBUBOX_INCLUDE_CONFIG_FILES_IF_EXISTS()

LIBUBOX_WRAP_SOURCELIST(${LibUbox_SOURCES})

LIBUBOX_COPY_FILES(LibUbox_CopyPrivateHeaders
        DESTINATION ${LibUbox_PRIVATE_FRAMEWORK_HEADERS_DIR}/libubox
        FILES ${LibUbox_PRIVATE_FRAMEWORK_HEADERS}
        FLATTENED
)
list(APPEND LibUbox_INTERFACE_DEPENDENCIES LibUbox_CopyPrivateHeaders)

if(NOT INTERNAL_BUILD)
        add_dependencies(LibUbox_CopyPrivateHeaders LibUbox)
endif()

# 根据以上设定定义框架（函数库）的源文件以及目标名称等。
# 其中会用到 source/cmake/target/LibUbox.cmake 定义的目标名称，
# 这里是 LibUbox::LibUbox
LIBUBOX_COMPUTE_SOURCES(LibUbox)
LIBUBOX_FRAMEWORK(LibUbox)
LIBUBOX_FRAMEWORK_TARGET(LibUbox)

if(${LibUbox_LIBRARY_TYPE} MATCHES "SHARED")
        set_target_properties(LibUbox PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})
        install(TARGETS LibUbox DESTINATION "${LIB_INSTALL_DIR}")
endif()

if(MSVC)
        set_target_properties(LibUbox PROPERTIES
                COMPILE_PDB_NAME ${LibUbox_OUTPUT_NAME}
        )
endif()

# 安装 LibUbox 目标到系统函数库路径，默认是 /usr/local/lib
install(TARGETS LibUbox
        DESTINATION "${LIB_INSTALL_DIR}/"
)