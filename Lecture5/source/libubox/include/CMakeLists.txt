# 列出要安装到系统中的头文件
set(LibUbox_FRAMEWORK_HEADERS
        "${LIBUBOX_DIR}/include/*.h"
        "${LIBUBOX_DIR}/include/private/*.h"
)

set(LibUbox_INSTALLED_HEADERS ${LibUbox_FRAMEWORK_HEADERS})
# 安装 HelloWorld_FRAMEWORK_HEADERS 变量定义的所有头文件到指定的系统路径
install(FILES ${LibUbox_INSTALLED_HEADERS}
        DESTINATION "${HEADER_INSTALL_DIR}/libubox"
)