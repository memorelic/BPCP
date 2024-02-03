cmake_minimum_required(VERSION 3.15)

project(CKeyword LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH ./cmake/)

find_package(GLIB2 REQUIRED)

add_executable(isCkeyword)

file(GLOB sources CONFIGURE_DEPENDS ./*.c)

target_sources(isCkeyword PRIVATE ${sources})

target_include_directories(isCkeyword PRIVATE GLIB2_INCLUDE_DIRS)

target_link_libraries(isCkeyword GLIB2::GLIB2)


