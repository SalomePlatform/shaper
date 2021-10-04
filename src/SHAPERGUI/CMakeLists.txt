# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

INCLUDE(UseQtExt)

# additional include directories
INCLUDE_DIRECTORIES(${QT_INCLUDES})

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS})

SET(UPDATE_TRANSLATION OFF)

SET(PROJECT_HEADERS
    SHAPER_SHAPERGUI.h
    SHAPERGUI.h
    SHAPERGUI_DataModel.h
    SHAPERGUI_OCCSelector.h
    SHAPERGUI_SalomeViewer.h
    SHAPERGUI_NestedButton.h
    SHAPERGUI_ToolbarsMgr.h
)

SET(PROJECT_MOC_HEADERS
    SHAPERGUI.h
    SHAPERGUI_DataModel.h
    SHAPERGUI_NestedButton.h
    SHAPERGUI_SalomeViewer.h
    SHAPERGUI_ToolbarsMgr.h
)

# sources / moc wrappings
QT_WRAP_MOC(PROJECT_AUTOMOC ${PROJECT_MOC_HEADERS})

SET(PROJECT_SOURCES
    SHAPERGUI.cpp
    SHAPERGUI_DataModel.cpp
    SHAPERGUI_OCCSelector.cpp
    SHAPERGUI_SalomeViewer.cpp
    SHAPERGUI_NestedButton.cpp
    SHAPERGUI_ToolbarsMgr.cpp
)

SET(PROJECT_RESOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/LightApp.xml
    resources/SalomeApp.xml
    resources/shaper.png
)

SET(TEXT_RESOURCES
    SHAPERGUI_msg_fr.ts
)

IF (${UPDATE_TRANSLATION})
    SET(PROJECT_FILES ${PROJECT_SOURCES} ${PROJECT_HEADERS} )
    QT5_CREATE_TRANSLATION(QM_RESOURCES
                           ${PROJECT_FILES}
                           ${TEXT_RESOURCES}
                           OPTIONS -extensions cpp -no-recursive -locations none
                          )
ELSE(${UPDATE_TRANSLATION})
    IF(${MAKE_TRANSLATION})
        QT5_ADD_TRANSLATION(QM_RESOURCES ${TEXT_RESOURCES})
    ENDIF(${MAKE_TRANSLATION})
ENDIF(${UPDATE_TRANSLATION})


SET(PROJECT_LIBRARIES
    Events
    Config
    XGUI
    ModuleBase
    ${QT_LIBRARIES}
    ${suit}
    ${std}
    ${LightApp}
    ${CAM}
    ${OCCViewer}
    ${qtx}
    ${SalomeObject}
)

ADD_DEFINITIONS( -DSHAPERGUI_EXPORTS ${OpenCASCADE_DEFINITIONS} )

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/XGUI
                    ${PROJECT_SOURCE_DIR}/src/Events
                    ${PROJECT_SOURCE_DIR}/src/ModuleBase
                    ${PROJECT_SOURCE_DIR}/src/ModelAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomAPI
                    ${PROJECT_SOURCE_DIR}/src/Config
                    ${PROJECT_SOURCE_DIR}/src/ExchangePlugin
                    ${SUIT_INCLUDE}
                    ${OpenCASCADE_INCLUDE_DIR}
)

ADD_LIBRARY(SHAPER SHARED
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    ${PROJECT_AUTOMOC}
    ${QM_RESOURCES}
)

ADD_DEPENDENCIES(SHAPER XGUI)

# The Qt5Widgets_LIBRARIES variable also includes QtGui and QtCore
TARGET_LINK_LIBRARIES(SHAPER ${PROJECT_LIBRARIES})

#
add_executable(shapergui_app shapergui_app.cpp)
target_link_libraries(shapergui_app ${QT_LIBRARIES})

# configuration
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/resources/LightApp.xml.in"
  "${CMAKE_CURRENT_BINARY_DIR}/LightApp.xml"
  )

install(TARGETS shapergui_app DESTINATION ${SHAPER_INSTALL_EXE})
INSTALL(TARGETS SHAPER DESTINATION ${SHAPER_INSTALL_BIN})
INSTALL(FILES ${PROJECT_RESOURCES} DESTINATION ${SHAPER_INSTALL_RESOURCES})
install(FILES resources/SalomeApp.xml RENAME SalomeAppSL.xml DESTINATION ${SHAPER_INSTALL_RESOURCES})
INSTALL(FILES ${QM_RESOURCES} DESTINATION ${SHAPER_INSTALL_QM_RESOURCES})
