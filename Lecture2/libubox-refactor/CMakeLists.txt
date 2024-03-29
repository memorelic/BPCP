cmake_minimum_required(VERSION 2.6)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckFunctionExists)

PROJECT(ubox C)

ADD_DEFINITIONS(-Wall -Werror)
IF(CMAKE_C_COMPILER_VERSION VERSION_GREATER 6)
	ADD_DEFINITIONS(-Wextra -Werror=implicit-function-declaration)
	ADD_DEFINITIONS(-Wformat -Werror=format-security -Werror=format-nonliteral)
ENDIF()
ADD_DEFINITIONS(-Os -std=gnu99 -g3 -Wmissing-declarations -Wno-unused-parameter)

include_directories(./src/lib/include)

OPTION(BUILD_LUA "build Lua plugin" OFF)
OPTION(BUILD_EXAMPLES "build examples" OFF)

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(JSONC json-c)
IF(JSONC_FOUND)
  ADD_DEFINITIONS(-DJSONC)
  INCLUDE_DIRECTORIES(${JSONC_INCLUDE_DIRS})
ENDIF()

file(GLOB SOURCES CONFIGURE_DEPENDS
	./src/lib/avl/*.c
	./src/lib/blob/*.c
	./src/lib/crypto/*.c
	#./src/lib/json/*.c
	./src/lib/kvlist/*.c
	./src/lib/runqueue/*.c
	./src/lib/safe_list/*.c
	./src/lib/ulog/*.c
	./src/lib/uloop/uloop.c
	./src/lib/usock/*.c
	./src/lib/ustream/*.c
	./src/lib/util/*.c
	./src/lib/vlist/*.c
)

#SET(SOURCES avl.c avl-cmp.c blob.c blobmsg.c uloop.c usock.c ustream.c ustream-fd.c vlist.c utils.c safe_list.c runqueue.c md5.c kvlist.c ulog.c base64.c)

ADD_LIBRARY(ubox SHARED ${SOURCES})
ADD_LIBRARY(ubox-static STATIC ${SOURCES})
SET_TARGET_PROPERTIES(ubox-static PROPERTIES OUTPUT_NAME ubox)

SET(LIBS)
CHECK_FUNCTION_EXISTS(clock_gettime HAVE_GETTIME)
IF(NOT HAVE_GETTIME)
	CHECK_LIBRARY_EXISTS(rt clock_gettime "" NEED_GETTIME)
	IF(NEED_GETTIME)
		TARGET_LINK_LIBRARIES(ubox rt)
	ENDIF()
ENDIF()

FILE(GLOB headers src/lib/include/*.h)
INSTALL(FILES ${headers}
	DESTINATION include/libubox
)
INSTALL(TARGETS ubox ubox-static
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
)

ADD_SUBDIRECTORY(./src/lua)
ADD_SUBDIRECTORY(./src/examples)

MACRO(ADD_UNIT_TEST_SAN name)
  ADD_EXECUTABLE(${name}-san ${name}.c)
  TARGET_COMPILE_OPTIONS(${name}-san PRIVATE -g -fno-omit-frame-pointer -fsanitize=undefined,address,leak -fno-sanitize-recover=all)
  TARGET_LINK_OPTIONS(${name}-san PRIVATE -fsanitize=undefined,address,leak)
  TARGET_LINK_LIBRARIES(${name}-san ubox blobmsg_json json_script ${json})
  TARGET_INCLUDE_DIRECTORIES(${name}-san PRIVATE ${PROJECT_SOURCE_DIR}/src/lib/include)
ENDMACRO(ADD_UNIT_TEST_SAN)

IF(UNIT_TESTING)
  ENABLE_TESTING()
  ADD_SUBDIRECTORY(./src/tests)
ENDIF()

find_library(json NAMES json-c)
IF(EXISTS ${json})
	ADD_LIBRARY(blobmsg_json SHARED src/lib/blob/blobmsg_json.c)
	TARGET_LINK_LIBRARIES(blobmsg_json ubox ${json})

	ADD_LIBRARY(blobmsg_json-static STATIC src/lib/blob/blobmsg_json.c)
	SET_TARGET_PROPERTIES(blobmsg_json-static
			      PROPERTIES OUTPUT_NAME blobmsg_json)

	IF(UNIT_TESTING)
		ADD_UNIT_TEST_SAN(jshn)
	ENDIF(UNIT_TESTING)

	ADD_EXECUTABLE(jshn src/lib/json/jshn.c)
	TARGET_LINK_LIBRARIES(jshn blobmsg_json ${json})

	ADD_LIBRARY(json_script SHARED src/lib/json/json_script.c)
	TARGET_LINK_LIBRARIES(json_script ubox)

	INSTALL(TARGETS blobmsg_json blobmsg_json-static jshn json_script
		ARCHIVE DESTINATION lib
		LIBRARY DESTINATION lib
		RUNTIME DESTINATION bin
	)

	FILE(GLOB scripts src/sh/*.sh)
	INSTALL(FILES ${scripts}
		DESTINATION share/libubox
	)

ENDIF()

IF(ABIVERSION)
	SET_TARGET_PROPERTIES(ubox PROPERTIES VERSION ${ABIVERSION})
	SET_TARGET_PROPERTIES(json_script PROPERTIES VERSION ${ABIVERSION})
	SET_TARGET_PROPERTIES(blobmsg_json PROPERTIES VERSION ${ABIVERSION})
ENDIF()
