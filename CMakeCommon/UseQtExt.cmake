# Copyright (C) 2007-2019  CEA/DEN, EDF R&D, OPEN CASCADE
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

IF (QT_VERSION VERSION_LESS "5.0")
  INCLUDE(${QT_USE_FILE})
ENDIF()

# This MACRO uses the following vars
# - QT_LRELEASE_EXECUTABLE : (given by default by FindQT.cmake)
#
# tsfiles containing all ts files to be compiled.
# installdir contains directory (relative to install_prefix) where to install files after compilation of ts files too qm.
MACRO(QT_INSTALL_TS_RESOURCES tsfiles installdir)
  FOREACH(_input ${tsfiles})
    GET_FILENAME_COMPONENT(_name ${_input} NAME_WE)
    SET(_output ${CMAKE_CURRENT_BINARY_DIR}/${_name}.qm)
    SET(_cmd_${_name} ${QT_LRELEASE_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${_input} -qm ${_output})
    ADD_CUSTOM_TARGET(QT_INSTALL_TS_RESOURCES_${_name} ALL COMMAND ${_cmd_${_name}} DEPENDS ${_input})
    INSTALL(FILES ${_output} DESTINATION ${installdir})
  ENDFOREACH()
ENDMACRO(QT_INSTALL_TS_RESOURCES)

MACRO(QT_WRAP_MOC)
IF (QT_VERSION VERSION_LESS "5.0")
  QT4_WRAP_CPP(${ARGN})
ELSE()
  QT5_WRAP_CPP(${ARGN})  
  # Workaround to avoid problem with bug in moc preprocessor:
  # The problem is reproduced on Linux in case if native system Qt is installed
  # and "-I/usr/include" parameter is presented in the moc command line before
  # custom Qt includes. This bug takes place in Qt-5.6.0 and newer. To avoid this
  # bug move "-I/usr/include" parameter in the "moc parameters" file to the end
  # of the "include section".
  IF (NOT WIN32)
    FOREACH(IT ${ARGN})
      GET_FILENAME_COMPONENT(IT ${IT} ABSOLUTE)
      GET_FILENAME_COMPONENT(PFILE ${IT} NAME_WE)
      SET(OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/moc_${PFILE}.cpp_parameters)
      IF(EXISTS ${OUTFILE})      
        SET(NEW_CONTENT)
	SET(IS_INCLUDE_SECTION TRUE)
	SET(HAS_SYSTEM_INC FALSE)
        FILE(READ ${OUTFILE} CONTENT)
	STRING(REGEX REPLACE "\n" ";" CONTENT "${CONTENT}")
	list(REMOVE_DUPLICATES CONTENT)
        FOREACH(S ${CONTENT})
          IF("${S}" MATCHES "^-I")
	    IF("${S}" STREQUAL "-I/usr/include")
	      SET(HAS_SYSTEM_INC TRUE)
	    ELSE()
	      SET(NEW_CONTENT ${NEW_CONTENT} "${S}\n")
	    ENDIF()
	  ELSE()
	    SET(IS_INCLUDE_SECTION FALSE)
	  ENDIF()
	  IF(NOT IS_INCLUDE_SECTION)
	    IF(HAS_SYSTEM_INC)
    	      SET(NEW_CONTENT ${NEW_CONTENT} "-I/usr/include\n")
	      SET(HAS_SYSTEM_INC FALSE)
	    ENDIF()
	    SET(NEW_CONTENT ${NEW_CONTENT} "${S}\n")
	  ENDIF()
        ENDFOREACH()
        FILE(WRITE ${OUTFILE} ${NEW_CONTENT})	
      ENDIF()
    ENDFOREACH()
  ENDIF()
ENDIF()
ENDMACRO(QT_WRAP_MOC)

MACRO(QT_WRAP_UIC)
IF (QT_VERSION VERSION_LESS "5.0")
  QT4_WRAP_UI(${ARGN})
ELSE()
  QT5_WRAP_UI(${ARGN})
ENDIF()
ENDMACRO(QT_WRAP_UIC)

MACRO(QT_ADD_RESOURCES)
IF (QT_VERSION VERSION_LESS "5.0")
  QT4_ADD_RESOURCES(${ARGN})
ELSE()
  QT5_ADD_RESOURCES(${ARGN})
ENDIF()
ENDMACRO(QT_ADD_RESOURCES)
