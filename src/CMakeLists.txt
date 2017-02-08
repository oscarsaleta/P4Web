find_package ( Boost 1.40 COMPONENTS system signals REQUIRED )

file (GLOB files_src
  "*.h"
  "*.cc")

set (WT_PROJECT_SOURCE
  ${files_src}
  )
link_directories(${WT_LIB_DIR})

set (WT_PROJECT_TARGET WP4.wt)

add_executable(${WT_PROJECT_TARGET} ${WT_PROJECT_SOURCE})

if (${CMAKE_MAJOR_VERSION} EQUAL 3)
    message (STATUS "CMake version >= 3.0 detected")
    target_compile_features (${WT_PROJECT_TARGET} PRIVATE cxx_nullptr)
else()
    message (STATUS "CMake version < 3.0 detected")
    set (CMAKE_CXX_FLAGS "-std=c++11")
endif()

target_link_libraries(${WT_PROJECT_TARGET} ${Boost_LIBRARIES} ${WT_CONNECTOR} wtdbo wtdbosqlite3 wt ${Boost_LIBRARIES})

include_directories(${WT_INCLUDE_DIR} ${Boost_INCLUDE_DIR})