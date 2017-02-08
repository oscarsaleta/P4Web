cmake_minimum_required (VERSION 3.0)

project(WP4)
set (WP4_VERSION_MAJOR 0)
set (WP4_VERSION_MINOR 1)
set (WP4_VERSION_PATCH 0)

set (CMAKE_CXX_FLAGS "-std=c++11")

set (WT_CONNECTOR
    wthttp wtfcgi
    CACHE STRING "Connector used (wthttp or wtfcgi)")

find_library (CONNECTOR_LIBRARY ${WT_CONNECTOR})

if (NOT CONNECTOR_LIBRARY)
    message (FATAL_ERROR "-- ${WT_CONNECTOR} library not found")
else ()
    message ("-- Found library ${WT_CONNECTOR}")
endif ()

add_subdirectory (src)
