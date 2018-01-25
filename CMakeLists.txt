## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.10)

PROJECT (SHAPER)
# Ensure a proper linker behavior:
CMAKE_POLICY(SET CMP0003 NEW)
IF(WIN32)
  CMAKE_POLICY(SET CMP0020 OLD) # disable automatic linking to qtmain.lib
ENDIF(WIN32)

SET (SHAPER_Version 3.0.0RC1)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeCommon" ${CMAKE_MODULE_PATH})

INCLUDE(SalomeMacros)

INCLUDE(FindEclipse)
INCLUDE(Common)
INCLUDE(FindSalomeQt5)
INCLUDE(FindPython)
INCLUDE(FindSalome)
INCLUDE(FindCAS)
INCLUDE(FindSUIT)
INCLUDE(FindTInspector)

# Find LibXml2
IF(DEFINED ENV{LIBXML2_ROOT_DIR})
    IF(WIN32)
        FILE(TO_CMAKE_PATH $ENV{LIBXML2_ROOT_DIR}/lib/libxml2.lib LIBXML2_LIBRARIES)
        FILE(TO_CMAKE_PATH $ENV{LIBXML2_ROOT_DIR}/include LIBXML2_INCLUDE_DIR)
    ELSE()
        FILE(TO_CMAKE_PATH $ENV{LIBXML2_ROOT_DIR}/lib/libxml2.so LIBXML2_LIBRARIES)
        FILE(TO_CMAKE_PATH $ENV{LIBXML2_ROOT_DIR}/include/libxml2 LIBXML2_INCLUDE_DIR)
    ENDIF()
ENDIF()
# It could be called only once
FIND_PACKAGE(LibXml2 REQUIRED)

SET(_pydir lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages/salome)
SET(SHAPER_INSTALL_PYTHON_API ${_pydir}/salome/shaper CACHE INTERNAL "" FORCE)
SET(SHAPER_INSTALL_SWIG ${_pydir} CACHE INTERNAL "" FORCE)
IF(${HAVE_SALOME})
    SET(SHAPER_INSTALL_ADDONS bin/salome CACHE INTERNAL "" FORCE)
    SET(SHAPER_INSTALL_BIN lib/salome CACHE INTERNAL "" FORCE)
    SET(SHAPER_INSTALL_PLUGIN_FILES lib/salome CACHE INTERNAL "" FORCE)
    SET(SHAPER_INSTALL_PYTHON_FILES bin/salome CACHE INTERNAL "" FORCE)
    SET(SHAPER_INSTALL_RESOURCES share/salome/resources/shaper CACHE INTERNAL "" FORCE)
    SET(SHAPER_INSTALL_QM_RESOURCES share/salome/resources/shaper CACHE INTERNAL "" FORCE)
    SET(SHAPER_INSTALL_XML_RESOURCES share/salome/resources/shaper CACHE INTERNAL "" FORCE)
    SET(SHAPER_INSTALL_GUI_DOC share/doc/salome/gui/SHAPER CACHE INTERNAL "" FORCE)
    SET(SHAPER_INSTALL_TUI_DOC share/doc/salome/tui/SHAPER CACHE INTERNAL "" FORCE)
    # without SALOME there is another kind of documentation, separated one
    ADD_SUBDIRECTORY (doc)
ELSE(${HAVE_SALOME})
    SET(SHAPER_INSTALL_ADDONS addons CACHE INTERNAL "" FORCE)
    SET(SHAPER_INSTALL_BIN bin CACHE INTERNAL "" FORCE)
    SET(SHAPER_INSTALL_PLUGIN_FILES plugins CACHE INTERNAL "" FORCE)
    SET(SHAPER_INSTALL_PYTHON_FILES plugins CACHE INTERNAL "" FORCE)
    SET(SHAPER_INSTALL_RESOURCES resources CACHE INTERNAL "" FORCE)
    SET(SHAPER_INSTALL_QM_RESOURCES bin CACHE INTERNAL "" FORCE)
    SET(SHAPER_INSTALL_XML_RESOURCES plugins CACHE INTERNAL "" FORCE)
    SET(SHAPER_INSTALL_GUI_DOC doc CACHE INTERNAL "" FORCE)
    SET(SHAPER_INSTALL_TUI_DOC doc CACHE INTERNAL "" FORCE)
ENDIF(${HAVE_SALOME})

# Sketcher: Change radius of circular edges while dragging a point on the edge
SET(SKETCHER_CHANGE_RADIUS_WHEN_MOVE FALSE)

ADD_SUBDIRECTORY (src/Config)
ADD_SUBDIRECTORY (src/Events)
ADD_SUBDIRECTORY (src/Model)
ADD_SUBDIRECTORY (src/XAO)
ADD_SUBDIRECTORY (src/GeomAPI)
ADD_SUBDIRECTORY (src/ModelAPI)
ADD_SUBDIRECTORY (src/ModelGeomAlgo)
ADD_SUBDIRECTORY (src/GeomAlgoAPI)
ADD_SUBDIRECTORY (src/GeomAlgoImpl)
ADD_SUBDIRECTORY (src/GeomData)
ADD_SUBDIRECTORY (src/GeomDataAPI)
ADD_SUBDIRECTORY (src/PartSetPlugin)
ADD_SUBDIRECTORY (src/ConstructionPlugin)
ADD_SUBDIRECTORY (src/BuildPlugin)
ADD_SUBDIRECTORY (src/PrimitivesPlugin)
ADD_SUBDIRECTORY (src/GDMLPlugin)
ADD_SUBDIRECTORY (src/FeaturesPlugin)
ADD_SUBDIRECTORY (src/CollectionPlugin)
ADD_SUBDIRECTORY (src/SamplePanelPlugin)
ADD_SUBDIRECTORY (src/SketcherPrs)
ADD_SUBDIRECTORY (src/SketchPlugin)
ADD_SUBDIRECTORY (src/SketchSolver)
ADD_SUBDIRECTORY (src/ModuleBase)
ADD_SUBDIRECTORY (src/PartSet)

ADD_SUBDIRECTORY (src/XGUI)
ADD_SUBDIRECTORY (src/ExchangePlugin)
ADD_SUBDIRECTORY (src/GeomValidators)
ADD_SUBDIRECTORY (src/InitializationPlugin)
ADD_SUBDIRECTORY (src/ParametersPlugin)
ADD_SUBDIRECTORY (src/PythonAddons)
ADD_SUBDIRECTORY (src/PythonAPI)
# High Level C++/Python API
ADD_SUBDIRECTORY (src/ModelHighAPI)
ADD_SUBDIRECTORY (src/BuildAPI)
ADD_SUBDIRECTORY (src/ConstructionAPI)
ADD_SUBDIRECTORY (src/ExchangeAPI)
ADD_SUBDIRECTORY (src/FeaturesAPI)
ADD_SUBDIRECTORY (src/CollectionAPI)
ADD_SUBDIRECTORY (src/ParametersAPI)
ADD_SUBDIRECTORY (src/PartSetAPI)
ADD_SUBDIRECTORY (src/PrimitivesAPI)
ADD_SUBDIRECTORY (src/SketchAPI)
ADD_SUBDIRECTORY (src/GDMLAPI)
ADD_SUBDIRECTORY (src/ConnectorAPI)
# Tests
ADD_SUBDIRECTORY (test.API/SHAPER)

IF(${HAVE_SALOME})
    ADD_SUBDIRECTORY (src/SHAPERGUI)
    ADD_SUBDIRECTORY (src/ConnectorPlugin)
ENDIF(${HAVE_SALOME})

ENABLE_TESTING()

IF(ADD_MODELS_TESTS)
  ADD_SUBDIRECTORY (test.models)
  ADD_CUSTOM_TARGET(run_unit_tests COMMAND ${CMAKE_CTEST_COMMAND} -C "${CMAKE_BUILD_TYPE}" -LE "models_tests")
ENDIF(ADD_MODELS_TESTS)

# Add the uninstall target for eclipse IDE
if (CMAKE_GENERATOR MATCHES "NMake Makefiles")
configure_file("${CMAKE_SOURCE_DIR}/CMakeCommon/cmake_uninstall.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
               IMMEDIATE @ONLY)
add_custom_target(uninstall
                  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif (CMAKE_GENERATOR MATCHES "NMake Makefiles")

# Add special Git hooks
if((EXISTS "${CMAKE_SOURCE_DIR}/.git") AND (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git"))
  if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/.git")
    set(GIT_SUBMODULE_PATH ".git")
  else()
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/.git" GIT_SUBMODULE_PATH)
    string(REGEX REPLACE "gitdir: " "" GIT_SUBMODULE_PATH "${GIT_SUBMODULE_PATH}")
    string(REGEX REPLACE "\n" "" GIT_SUBMODULE_PATH "${GIT_SUBMODULE_PATH}")
  endif()

  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/GitHooks/pre-commit.in"
                 "${CMAKE_CURRENT_SOURCE_DIR}/${GIT_SUBMODULE_PATH}/hooks/pre-commit")
endif()
