find_package ( Boost 1.40 COMPONENTS system REQUIRED )

file (GLOB files_src
  "*.h"
  "*.cc")

set (WT_PROJECT_SOURCE
  ${files_src}
  )

set (WT_PROJECT_TARGET WP4.wt)

add_executable(${WT_PROJECT_TARGET} ${WT_PROJECT_SOURCE})

target_link_libraries(${WT_PROJECT_TARGET} ${Boost_LIBRARIES} ${WT_CONNECTOR} wtdbo wtdbosqlite3 wt )

include_directories(/usr/local/include/Wt ${Boost_INCLUDE_DIR})