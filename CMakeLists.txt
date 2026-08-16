cmake_minimum_required (VERSION 3.16)

project (LoadLineCalculator
  VERSION 1.0.0
  LANGUAGES CXX
)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(LLC_VERSION_STR "v${PROJECT_VERSION}")
string(TIMESTAMP LLC_VERSION_DATE_STR "%d/%m/%Y, %H:%M:%S %Z" UTC)
file(GLOB TUBE_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/share/LoadLineCalculator/tubes/*.json")
list(LENGTH TUBE_FILES TUBE_FILE_COUNT)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets PrintSupport)
qt_standard_project_setup()

include(FetchContent)

if (NOT DEFINED QCP_VERSION)
  set(QCP_VERSION 2.1.1)
endif()
message(STATUS "Targeting QCustomPlot version: ${QCP_VERSION}")

FetchContent_Declare(
  QCustomPlot
  URL https://www.qcustomplot.com/release/${QCP_VERSION}/QCustomPlot-source.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_GetProperties(QCustomPlot)
if (NOT qcustomplot_POPULATED)
  FetchContent_Populate(QCustomPlot)

  file(WRITE "${qcustomplot_SOURCE_DIR}/CMakeLists.txt"
    "
      cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})
      project(QCustomPlot LANGUAGES CXX)
      find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets PrintSupport)
      add_library(QCustomPlotLib
        STATIC
        qcustomplot.cpp
        qcustomplot.h
      )
      if(WIN32)
        if(CMAKE_CXX_COMPILER_ID STREQUAL \"GNU\" OR CMAKE_CXX_COMPILER_ID STREQUAL \"Clang\")
          target_compile_options(QCustomPlotLib PRIVATE -Wa,-mbig-obj)
        elseif(MSVC)
          target_compile_options(QCustomPlotLib PRIVATE /bigobj)
        endif()
      endif()
      target_link_libraries(QCustomPlotLib
        PUBLIC
          Qt6::Core
          Qt6::Gui
          Qt6::Widgets
          Qt6::PrintSupport
      )
      target_include_directories(QCustomPlotLib
        PUBLIC
          \${CMAKE_CURRENT_SOURCE_DIR}
      )
      target_precompile_headers(QCustomPlotLib
        PUBLIC
          qcustomplot.h
      )
    "
  )
endif()
add_subdirectory(${qcustomplot_SOURCE_DIR} ${qcustomplot_BINARY_DIR})

set(LLC_ICO "${CMAKE_CURRENT_SOURCE_DIR}/src/resources/ico/llc_multi.ico")

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/loadlinecalculator.rc.in"
  "${CMAKE_CURRENT_BINARY_DIR}/loadlinecalculator.rc"
  @ONLY
)

set(INSTALLED_LLC_EXE "${CMAKE_INSTALL_PREFIX}/bin/LoadLineCalculator${CMAKE_EXECUTABLE_SUFFIX}")
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/pack_tubes.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/pack_tubes.cmake"
  @ONLY
)

set(LoadLineCalculator_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(LoadLineCalculator_SOURCES
  "${LoadLineCalculator_SOURCE_DIR}/AboutPage.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/Calculator.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/Config.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/eeutils.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/FirstTimeSetup.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/InterelectrodeCapacitances.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/InterpolatedCurve.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/IVCurve.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/main.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/mainwindow.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/ScalableSpinBoxValue.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/TubeDataDialog.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/TubeDataDialog.showCSVHelp.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/TubeHeaterInfo.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/TubeInfoEntry.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/TubeManager.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/TubeRatings.cpp"
)
set(LoadLineCalculator_HEADERS
  "${LoadLineCalculator_SOURCE_DIR}/AboutPage.hpp"
  "${LoadLineCalculator_SOURCE_DIR}/BaseValueLabel.hpp"
  "${LoadLineCalculator_SOURCE_DIR}/Calculator.hpp"
  "${LoadLineCalculator_SOURCE_DIR}/Config.hpp"
  "${LoadLineCalculator_SOURCE_DIR}/eeutils.hpp"
  "${LoadLineCalculator_SOURCE_DIR}/FirstTimeSetup.hpp"
  "${LoadLineCalculator_SOURCE_DIR}/InterpolatedCurve.hpp"
  "${LoadLineCalculator_SOURCE_DIR}/IntValueLabel.hpp"
  "${LoadLineCalculator_SOURCE_DIR}/IVCurve.hpp"
  "${LoadLineCalculator_SOURCE_DIR}/mainwindow.hpp"
  "${LoadLineCalculator_SOURCE_DIR}/ScalableSpinBoxValue.hpp"
  "${LoadLineCalculator_SOURCE_DIR}/TubeDataDialog.hpp"
  "${LoadLineCalculator_SOURCE_DIR}/TubeManager.hpp"
  "${LoadLineCalculator_SOURCE_DIR}/ValueLabel.hpp"
)
set(LoadLineCalculator_UI_SOURCES
  "${LoadLineCalculator_SOURCE_DIR}/csvhelp.ui"
  "${LoadLineCalculator_SOURCE_DIR}/mainwindow.ui"
  "${LoadLineCalculator_SOURCE_DIR}/tubedatadialog.ui"
  "${LoadLineCalculator_SOURCE_DIR}/loadlinecalculator.qrc"
  "${CMAKE_CURRENT_BINARY_DIR}/loadlinecalculator.rc"
)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  qt_add_executable(LoadLineCalculator
    ${LoadLineCalculator_SOURCES}
    ${LoadLineCalculator_HEADERS}
    ${LoadLineCalculator_UI_SOURCES}
  )
  target_compile_definitions(LoadLineCalculator
    PRIVATE
      DEBUG_BUILD
  )
else ()
  qt_add_executable(LoadLineCalculator
    WIN32
    ${LoadLineCalculator_SOURCES}
    ${LoadLineCalculator_HEADERS}
    ${LoadLineCalculator_UI_SOURCES}
  )
endif ()
target_link_libraries(LoadLineCalculator
  PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::PrintSupport
    Qt6::Gui
    QCustomPlotLib
)
target_include_directories(LoadLineCalculator
  PRIVATE 
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/qcustomplot"
)
target_compile_options(LoadLineCalculator 
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/bigobj>
    $<$<CXX_COMPILER_ID:GNU>:-Wa,-mbig-obj>
    $<$<CXX_COMPILER_ID:Clang>:$<$<PLATFORM_ID:Windows>:-Wa,-mbig-obj>>
)
target_compile_definitions(LoadLineCalculator
  PRIVATE
    LLC_VERSION="${LLC_VERSION_STR}"
    LLC_VERSION_DATE="${LLC_VERSION_DATE_STR}"
    TUBE_FILE_COUNT=${TUBE_FILE_COUNT}
)

install(
  TARGETS LoadLineCalculator
  BUNDLE DESTINATION .
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

configure_file(
  "${CMAKE_SOURCE_DIR}/cmake/deploy_win.cmake.in"
  "${CMAKE_BINARY_DIR}/deploy_win.cmake"
  @ONLY
)

install(
  SCRIPT
    "${CMAKE_BINARY_DIR}/deploy_win.cmake"
)

install(
  SCRIPT
    "${CMAKE_BINARY_DIR}/pack_tubes.cmake"
)