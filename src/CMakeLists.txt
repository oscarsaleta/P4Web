find_package ( Boost 1.40 COMPONENTS system REQUIRED )

set (WT_PROJECT_SOURCE
  color.h
  color.cc
  custom.h
  file_tab.h
  file_tab.cc
  HomeLeft.h
  HomeLeft.cc
  HomeRight.h
  HomeRight.cc
  main.cc
  MainUI.h
  MainUI.cc
  math_changedir.cc
  math_intblowup.h
  math_intblowup.cc
  math_numerics.cc
  math_orbits.cc
  math_p4.h
  math_p4.cc
  math_polynom.h
  math_polynom.cc
  math_separatrice.h
  math_separatrice.cc
  MyApplication.h
  MyApplication.cc
  MyLogger.h
  MyLogger.cc
  plot_points.h
  plot_points.cc
  plot_tools.h
  plot_tools.cc
  win_sphere.h
  win_sphere.cc
  )

set (WT_PROJECT_TARGET P4Web.wt)

add_executable(${WT_PROJECT_TARGET} ${WT_PROJECT_SOURCE})

target_link_libraries(${WT_PROJECT_TARGET} ${WT_CONNECTOR} wt ${Boost_LIBRARIES})

include_directories(/usr/local/include/Wt ${Boost_INCLUDE_DIR})