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

file (GLOB files_src
  "*.h"
  "*.cc")

set (WT_PROJECT_SOURCE
  ${files_src}
  )
link_directories(${WT_LIB_DIR})

set (WT_PROJECT_TARGET WP4.wt)

add_executable(${WT_PROJECT_TARGET} ${WT_PROJECT_SOURCE})

if (${COMPILED_ANTZ})
    message (STATUS "Compiling for antz: adding ssh call to Maple execution")
    add_definitions(-DANTZ)
endif()

if (${CMAKE_MAJOR_VERSION} EQUAL 3 AND ${CMAKE_MINOR_VERSION} GREATER 0)
    message (STATUS "CMake version > 3.0 detected")
    target_compile_features (${WT_PROJECT_TARGET} PRIVATE cxx_nullptr)
else()
    message (STATUS "CMake version <= 3.0 detected")
    set (CMAKE_CXX_FLAGS "-std=c++11")
endif()

target_link_libraries(${WT_PROJECT_TARGET} ${Boost_LIBRARIES} ${WT_CONNECTOR} wtdbo wtdbosqlite3 wt ${Boost_LIBRARIES})

include_directories(${WT_INCLUDE_DIR})