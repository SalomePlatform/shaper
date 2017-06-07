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

INCLUDE(Common)
INCLUDE(FindPython)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/Events
                    ${PROJECT_SOURCE_DIR}/src/Config
                    ${PROJECT_SOURCE_DIR}/src/ModelAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
                    ${PROJECT_SOURCE_DIR}/src/ParametersPlugin
                    ${SUIT_INCLUDE}
                    ${PYTHON_INCLUDE_DIR}
)

SET(PROJECT_HEADERS
    InitializationPlugin.h
    InitializationPlugin_Plugin.h
    InitializationPlugin_EvalListener.h
    InitializationPlugin_PyInterp.h
)

SET(PROJECT_SOURCES
    InitializationPlugin_Plugin.cpp
    InitializationPlugin_EvalListener.cpp
    InitializationPlugin_PyInterp.cpp
)

MESSAGE(STATUS "PYTHON_LIBRARIES (ParametersPlugin): ${PYTHON_LIBRARIES}")

SET(PROJECT_LIBRARIES
    Events
    Config
    ModelAPI
    ${PyInterp}
    ${PYTHON_LIBRARIES}
)

ADD_DEFINITIONS(-DINITIALIZATIONPLUGIN_EXPORTS -DHAVE_DEBUG_PYTHON)
ADD_LIBRARY(InitializationPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})

TARGET_LINK_LIBRARIES(InitializationPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS InitializationPlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
#INSTALL(FILES ${XML_RESOURCES} DESTINATION plugins)

