# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

SET(PROJECT_HEADERS
    XGUI.h
    XGUI_ActionsMgr.h
    XGUI_ActiveControlMgr.h
    XGUI_ActiveControlSelector.h
    XGUI_ColorDialog.h
    XGUI_ContextMenuMgr.h
    XGUI_CustomPrs.h
    XGUI_DataModel.h
    XGUI_DeflectionDialog.h
    XGUI_Displayer.h
    XGUI_ErrorDialog.h
    XGUI_ErrorMgr.h
    XGUI_FacesPanel.h
    XGUI_FacesPanelSelector.h
    XGUI_HistoryMenu.h
    XGUI_MenuGroup.h
    XGUI_MenuMgr.h
    XGUI_MenuWorkbench.h
    XGUI_ModuleConnector.h
    XGUI_ObjectsBrowser.h
    XGUI_OperationMgr.h
    XGUI_PropertyDialog.h
    XGUI_PropertyPanel.h
    XGUI_PropertyPanelSelector.h
    XGUI_QtEvents.h
    XGUI_SalomeConnector.h
    XGUI_Selection.h
    XGUI_SelectionActivate.h
    XGUI_SelectionFilterType.h
    XGUI_SelectionMgr.h
    XGUI_Tools.h
    XGUI_TransparencyWidget.h
    XGUI_ViewerProxy.h
    XGUI_Workshop.h
    XGUI_WorkshopListener.h
	XGUI_InspectionPanel.h
	XGUI_CompressFiles.h
)

SET(PROJECT_MOC_HEADERS
    XGUI_ActionsMgr.h
    XGUI_ActiveControlMgr.h
    XGUI_ActiveControlSelector.h
    XGUI_ColorDialog.h
    XGUI_ContextMenuMgr.h
    XGUI_DataModel.h
    XGUI_DeflectionDialog.h
    XGUI_Displayer.h
    XGUI_ErrorDialog.h
    XGUI_ErrorMgr.h
    XGUI_FacesPanel.h
    XGUI_FacesPanelSelector.h
    XGUI_HistoryMenu.h
    XGUI_ModuleConnector.h
    XGUI_ObjectsBrowser.h
    XGUI_OperationMgr.h
    XGUI_PropertyDialog.h
    XGUI_PropertyPanel.h
    XGUI_PropertyPanelSelector.h
    XGUI_SelectionMgr.h
    XGUI_TransparencyWidget.h
    XGUI_ViewerProxy.h
    XGUI_Workshop.h
    XGUI_WorkshopListener.h
	XGUI_InspectionPanel.h
)

# sources / moc wrappings
QT_WRAP_MOC(PROJECT_AUTOMOC ${PROJECT_MOC_HEADERS})

SET(PROJECT_SOURCES
    XGUI_ActionsMgr.cpp
    XGUI_ActiveControlMgr.cpp
    XGUI_ColorDialog.cpp
    XGUI_ContextMenuMgr.cpp
    XGUI_CustomPrs.cpp
    XGUI_DataModel.cpp
    XGUI_DeflectionDialog.cpp
    XGUI_Displayer.cpp
    XGUI_ErrorDialog.cpp
    XGUI_ErrorMgr.cpp
    XGUI_FacesPanel.cpp
    XGUI_FacesPanelSelector.cpp
    XGUI_HistoryMenu.cpp
    XGUI_MenuGroup.cpp
    XGUI_MenuMgr.cpp
    XGUI_MenuWorkbench.cpp
    XGUI_ModuleConnector.cpp
    XGUI_ObjectsBrowser.cpp
    XGUI_OperationMgr.cpp
    XGUI_PropertyDialog.cpp
    XGUI_PropertyPanel.cpp
    XGUI_PropertyPanelSelector.cpp
    XGUI_QtEvents.cpp
    XGUI_SalomeConnector.cpp
    XGUI_Selection.cpp
    XGUI_SelectionActivate.cpp
    XGUI_SelectionMgr.cpp
    XGUI_Tools.cpp
    XGUI_TransparencyWidget.cpp
    XGUI_ViewerProxy.cpp
    XGUI_Workshop.cpp
    XGUI_WorkshopListener.cpp
	XGUI_InspectionPanel.cpp
	XGUI_CompressFiles.cpp
)

SET(PROJECT_RESOURCES
    XGUI_pictures.qrc
)

SET(PREFERENCES_XML
    SHAPER.xml
)

SET(TEXT_RESOURCES
    XGUI_msg_fr.ts
)

SET(PROJECT_LIBRARIES
    Events
    Config
    ModelAPI
    ModuleBase
    ${QT_LIBRARIES}
    ${OpenCASCADE_FoundationClasses_LIBRARIES}
    ${OpenCASCADE_Visualization_LIBRARIES}
    ${PYTHON_LIBRARIES}
    ${PyConsole}
    ${PyInterp}
    ${suit}
    ${qtx}
)

IF(NOT ${HAVE_SALOME})
    SET(PROJECT_LIBRARIES ${PROJECT_LIBRARIES} AppElements)
ENDIF(NOT ${HAVE_SALOME})


IF(WIN32)
    SET(PROJECT_LIBRARIES ${PROJECT_LIBRARIES} opengl32)
ELSE()
    SET(PROJECT_LIBRARIES ${PROJECT_LIBRARIES} GL)
ENDIF()

QT_ADD_RESOURCES(PROJECT_COMPILED_RESOURCES ${PROJECT_RESOURCES})
#QT5_ADD_TRANSLATION(QM_RESOURCES ${TEXT_RESOURCES})
# QT4_CREATE_TRANSLATION(QM_RESOURCES
#                        ${PROJECT_SOURCES}
#                        ${TEXT_RESOURCES}
#                        OPTIONS -extensions cpp -no-recursive
#                        )


SOURCE_GROUP ("Generated Files" FILES ${PROJECT_AUTOMOC} ${PROJECT_COMPILED_RESOURCES} ${QM_RESOURCES})
#SOURCE_GROUP ("Generated Files" FILES ${PROJECT_AUTOMOC} ${PROJECT_COMPILED_RESOURCES})
SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES} ${PROJECT_RESOURCES} ${PREFERENCES_XML})

ADD_DEFINITIONS( -DXGUI_EXPORTS ${OpenCASCADE_DEFINITIONS} -D_CRT_SECURE_NO_WARNINGS)

SET(PROJECT_INCLUDES
    ${PROJECT_SOURCE_DIR}/src/Events
    ${PROJECT_SOURCE_DIR}/src/Config
    ${PROJECT_SOURCE_DIR}/src/ModelAPI
    ${PROJECT_SOURCE_DIR}/src/GeomAPI
    ${PROJECT_SOURCE_DIR}/src/ModuleBase
    ${PROJECT_SOURCE_DIR}/src/GeomAlgoAPI
    ${OpenCASCADE_INCLUDE_DIR}
    ${SUIT_INCLUDE})

IF(${HAVE_SALOME})
  INCLUDE_DIRECTORIES(${SALOME_KERNEL_INCLUDE})
ENDIF(${HAVE_SALOME})

IF(NOT ${HAVE_SALOME})
    SET(PROJECT_INCLUDES ${PROJECT_INCLUDES} ${APPELEMENTS_INCLUDE_DIR})
ENDIF(NOT ${HAVE_SALOME})

IF(TKTInspector)
    message("TINSPECTOR is defined")
    SET(PROJECT_LIBRARIES ${PROJECT_LIBRARIES} ${TKTInspectorAPI} ${TKTInspector} ${TKTreeModel} ${TKVInspector})
    SET(PROJECT_INCLUDES ${PROJECT_INCLUDES} ${TINSPECTOR_INCLUDE_DIR})

    ADD_DEFINITIONS(-DTINSPECTOR)
ENDIF()

INCLUDE_DIRECTORIES(${PROJECT_INCLUDES})

LINK_DIRECTORIES($ENV{PYTHON_LIB_DIR})

ADD_LIBRARY(XGUI SHARED
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    ${PROJECT_COMPILED_RESOURCES}
    ${TEXT_RESOURCES}
    ${QM_RESOURCES}
    ${PREFERENCES_XML}
    ${PROJECT_AUTOMOC}
)

ADD_DEPENDENCIES(XGUI ModelAPI)

# The Qt5Widgets_LIBRARIES variable also includes QtGui and QtCore
TARGET_LINK_LIBRARIES(XGUI ${PROJECT_LIBRARIES})

INSTALL(TARGETS XGUI DESTINATION ${SHAPER_INSTALL_BIN})
INSTALL(FILES ${QM_RESOURCES} DESTINATION ${SHAPER_INSTALL_QM_RESOURCES})
INSTALL(FILES ${PREFERENCES_XML} DESTINATION ${SHAPER_INSTALL_RESOURCES})
INSTALL(DIRECTORY icons/ DESTINATION ${SHAPER_INSTALL_RESOURCES})
