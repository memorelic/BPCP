# FIXME: These should line up with versions in Version.xcconfig
set(LIBUBOX_MAC_VERSION 0.0.1)
set(MACOSX_FRAMEWORK_BUNDLE_VERSION 0.0.1)

find_package(LibXml2 2.8.0)
find_package(LibXslt 1.1.7)
find_package(CURL 7.60.0)
find_package(OpenSSL 1.1.1)
find_package(SQLite3 3.10.0)

LIBUBOX_OPTION_BEGIN()
# Private options shared with other LibUbox ports. Add options here only if
# we need a value different from the default defined in GlobalFeatures.cmake.

LIBUBOX_OPTION_DEFAULT_PORT_VALUE(ENABLE_XML PUBLIC OFF)
LIBUBOX_OPTION_DEFAULT_PORT_VALUE(ENABLE_HTTP PUBLIC OFF)
LIBUBOX_OPTION_DEFAULT_PORT_VALUE(ENABLE_LSQL PUBLIC OFF)
LIBUBOX_OPTION_DEFAULT_PORT_VALUE(ENABLE_RSQL PUBLIC OFF)
LIBUBOX_OPTION_DEFAULT_PORT_VALUE(ENABLE_HIBUS PUBLIC OFF)
LIBUBOX_OPTION_DEFAULT_PORT_VALUE(ENABLE_SSL PUBLIC OFF)

LIBUBOX_OPTION_END()

set(LibUbox_PKGCONFIG_FILE ${CMAKE_BINARY_DIR}/src/libubox/libubox.pc)

set(LibUbox_LIBRARY_TYPE SHARED)
set(LibUboxTestSupport_LIBRARY_TYPE SHARED)

