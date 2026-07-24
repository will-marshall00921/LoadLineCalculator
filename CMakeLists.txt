cmake_minimum_required (VERSION 3.16)

project (LoadLineCalculator
  VERSION 0.2.0
  LANGUAGES CXX
)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets PrintSupport)
qt_standard_project_setup()

set(LoadLineCalculator_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(LoadLineCalculator_SOURCES
  "${LoadLineCalculator_SOURCE_DIR}/Calculator.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/Config.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/InterelectrodeCapacitances.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/InterpolatedCurve.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/IVCurve.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/main.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/mainwindow.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/ScalableSpinBoxValue.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/TubeHeaterInfo.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/TubeInfoEntry.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/TubeManager.cpp"
  "${LoadLineCalculator_SOURCE_DIR}/TubeRatings.cpp"
  "${CMAKE_SOURCE_DIR}/qcustomplot/qcustomplot.cpp"
)
set(LoadLineCalculator_HEADERS
  "${LoadLineCalculator_SOURCE_DIR}/BaseValueLabel.hpp"
  "${LoadLineCalculator_SOURCE_DIR}/Calculator.hpp"
  "${LoadLineCalculator_SOURCE_DIR}/Config.hpp"
  "${LoadLineCalculator_SOURCE_DIR}/InterpolatedCurve.hpp"
  "${LoadLineCalculator_SOURCE_DIR}/IntValueLabel.hpp"
  "${LoadLineCalculator_SOURCE_DIR}/IVCurve.hpp"
  "${LoadLineCalculator_SOURCE_DIR}/mainwindow.hpp"
  "${LoadLineCalculator_SOURCE_DIR}/ScalableSpinBoxValue.hpp"
  "${LoadLineCalculator_SOURCE_DIR}/TubeManager.hpp"
  "${LoadLineCalculator_SOURCE_DIR}/ValueLabel.hpp"
  "${CMAKE_SOURCE_DIR}/qcustomplot/qcustomplot.h"
)
set(LoadLineCalculator_UI_SOURCES
  "${LoadLineCalculator_SOURCE_DIR}/mainwindow.ui"
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

# install(
#   DIRECTORY
#     "${CMAKE_SOURCE_DIR}/share/LoadLineCalculator/tubes"
#   DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
# )

if (WIN32)
install(
  DIRECTORY
    "${CMAKE_SOURCE_DIR}/share/LoadLineCalculator/tubes"
  DESTINATION "$ENV{APPDATA}/LoadLineCalculator"
)
else()
  install(
    DIRECTORY
      "${CMAKE_SOURCE_DIR}/share/LoadLineCalculator/tubes"
    DESTINATION "$ENV{HOME}/.config"
  )
endif()