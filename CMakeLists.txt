cmake_minimum_required (VERSION 2.8)

project(WP4)
set (WP4_VERSION_MAJOR 1)
set (WP4_VERSION_MINOR 0)
set (WP4_VERSION_PATCH 0)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/macros")
include (ConfigureBoost)

set (WT_CONNECTOR
    wthttp wtfcgi
    CACHE STRING "Connector used (wthttp or wtfcgi)")

set (WT_LIB_DIR
    /usr/local/lib
    CACHE STRING "Location of Wt libs")

set (WT_INCLUDE_DIR
    /usr/local/include/Wt
    CACHE STRING "Location of Wt headers")

find_library (CONNECTOR_LIBRARY ${WT_CONNECTOR})

if (NOT CONNECTOR_LIBRARY)
    message (FATAL_ERROR "-- ${WT_CONNECTOR} library not found")
else ()
    message (STATUS "Found library ${WT_CONNECTOR}")
endif ()

add_subdirectory (src)
