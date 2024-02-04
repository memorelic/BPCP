if (NOT TARGET LibUbox::LibUbox)
    if (NOT INTERNAL_BUILD)
        message(FATAL_ERROR "LibUbox::LibUbox target not found")
    endif ()

    # This should be moved to an if block if the Apple Mac/iOS build moves completely to CMake
    # Just assuming Windows for the moment
    add_library(LibUbox::LibUbox STATIC IMPORTED)
    set_target_properties(LibUbox::LibUbox PROPERTIES
        IMPORTED_LOCATION ${WEBKIT_LIBRARIES_LINK_DIR}/LibUbox${DEBUG_SUFFIX}.lib
    )
    set(LibUbox_PRIVATE_FRAMEWORK_HEADERS_DIR "${CMAKE_BINARY_DIR}/../include/private")
    target_include_directories(LibUbox::LibUbox INTERFACE
        ${LibUbox_PRIVATE_FRAMEWORK_HEADERS_DIR}
    )
endif ()
