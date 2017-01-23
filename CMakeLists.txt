cmake_minimum_required (VERSION 3.0)

project(WP4)
set (WP4_VERSION_MAJOR 0)
set (WP4_VERSION_MINOR 1)
set (WP4_VERSION_PATCH 0)

set (WT_CONNECTOR "wthttp" CACHE STRING "Connector used (wthttp or wtfcgi)")

#set (CMAKE_CXX_FLAGS "-O3 -Wall")

add_subdirectory (src)
