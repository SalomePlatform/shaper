# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

IF (CMAKE_GENERATOR MATCHES "NMake Makefiles")
  IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    ADD_DEFINITIONS(-D_DEBUG)
  ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")
  IF(WIN32)
    FIND_PATH(VC_INCLUDE_PATH stdio.h)
    INCLUDE_DIRECTORIES(${VC_INCLUDE_PATH})
  ENDIF(WIN32)
ENDIF (CMAKE_GENERATOR MATCHES "NMake Makefiles")
