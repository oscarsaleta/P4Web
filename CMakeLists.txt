#  This file is part of WP4 (http://github.com/oscarsaleta/WP4)
#
#  Copyright (C) 2016  O. Saleta
#
#  WP4 is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published
#  by the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required (VERSION 2.8)

project(WP4)
set (WP4_VERSION_MAJOR 1)
set (WP4_VERSION_MINOR 1)
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
