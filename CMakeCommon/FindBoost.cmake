# Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
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

#find_package(Boost)

SET(BOOSTDIR $ENV{BOOST_ROOT_DIR})
IF(WIN32)
  INCLUDE_DIRECTORIES(${BOOSTDIR}/include/boost-1_52)
ELSE(WIN32)
  INCLUDE_DIRECTORIES(${BOOSTDIR}/include)
ENDIF()


LINK_DIRECTORIES (${BOOSTDIR}/lib)

#SET(BOOST_LIBRARIES ${BOOSTDIR}/lib/lib.lib)

IF(WIN32)
  SET(BOOST_DEFINITIONS -DBOOST_DISABLE_ASSERTS -DBOOST_ALL_DYN_LINK)
ENDIF()
