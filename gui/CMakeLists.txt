############################################################################
# CMakeLists.txt for building STeCaGUI
############################################################################

set(source_files
  app.cpp
  gui_helpers.cpp
  mainwin.cpp
  mainwin_parts.cpp
  models.cpp
  thehub.cpp
  panel/dataset.cpp
  panel/diffractogram.cpp
  panel/fitting.cpp
  panel/panel.cpp
  resources.qrc
)

set(QCustomPlot_source_files
  QCP/qcustomplot.cpp
)

include_directories(
  ${CMAKE_HOME_DIRECTORY}/core
  ${CMAKE_HOME_DIRECTORY}/gui
)

add_library(STeCaGUI
  ${source_files}
  ${QCustomPlot_source_files}
)

target_link_libraries(STeCaGUI
  Qt5::Core
  Qt5::Gui
  Qt5::PrintSupport
  Qt5::Widgets
  STeCaCore
  STeCaCoreIO
)

set_property(TARGET STeCaGUI PROPERTY CXX_STANDARD 11)
set_property(TARGET STeCaGUI PROPERTY CXX_STANDARD_REQUIRED ON)

add_executable(STeCa
  main.cpp
)

target_link_libraries(STeCa
  STeCaGUI
)

set_property(TARGET STeCa PROPERTY CXX_STANDARD 11)
set_property(TARGET STeCa PROPERTY CXX_STANDARD_REQUIRED ON)
