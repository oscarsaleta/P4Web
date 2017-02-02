cmake_minimum_required (VERSION 3.0)

project(WP4)
set (WP4_VERSION_MAJOR 0)
set (WP4_VERSION_MINOR 1)
set (WP4_VERSION_PATCH 0)

set (WT_CONNECTOR
    wthttp wtfcgi
    CACHE STRING "Connector used (wthttp or wtfcgi)")

add_subdirectory (src)
