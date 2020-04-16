# Copyright (C) 2013-2020  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
# Author: Roman NIKOLAEV
#

# OpenCascade detection for Salome
#
#  !! Please read the generic detection procedure in SalomeMacros.cmake !!
#

# TODO: RNV: Check different OCCT layouts !!!
SALOME_FIND_PACKAGE_AND_DETECT_CONFLICTS(OpenCASCADE OpenCASCADE_INCLUDE_DIR 2)

IF(OpenCASCADE_FOUND)

  IF(NOT CAS_FIND_QUIETLY)
      MESSAGE(STATUS "Found OpenCascade version: ${OpenCASCADE_VERSION}")
  ENDIF()

  # OPENCASCADE definitions
  SET(OpenCascade_DEFINITIONS ${OpenCASCADE_C_FLAGS} ${OpenCASCADE_CXX_FLAGS})
  SET(OpenCascade_LDFLAGS ${OpenCASCADE_LINKER_FLAGS})

  SALOME_ACCUMULATE_HEADERS(OpenCASCADE_INCLUDE_DIR)
  SALOME_ACCUMULATE_ENVIRONMENT(LD_LIBRARY_PATH ${OpenCASCADE_LIBRARY_DIR})

ELSE()
  # TODO: Detect OpenCASCADE if it is distributed without CMake configuration.
  IF(NOT CAS_FIND_QUIETLY)
    MESSAGE(STATUS "Could not find OpenCASCADE ...")
  ENDIF()
ENDIF()