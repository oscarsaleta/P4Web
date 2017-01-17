cmake_minimum_required (VERSION 3.0)

project(P4Web)
set (P4Web_VERSION_MAJOR 0)
set (P4Web_VERSION_MINOR 1)
set (P4Web_VERSION_PATCH 0)

set (WT_CONNECTOR "wthttp" CACHE STRING "Connector used (wthttp or wtfcgi)")

#set (CMAKE_CXX_FLAGS "-O3 -Wall")

add_subdirectory (src)
