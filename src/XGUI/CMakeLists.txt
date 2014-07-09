
SET(CMAKE_AUTOMOC ON)

SET(PROJECT_HEADERS
	XGUI.h
	XGUI_Command.h
	XGUI_Displayer.h
	XGUI_MainMenu.h
	XGUI_MainWindow.h
	XGUI_MenuGroupPanel.h
	XGUI_Tools.h
	XGUI_Workbench.h
	XGUI_Workshop.h
    XGUI_ViewWindow.h
    XGUI_ViewPort.h
    XGUI_Viewer.h
	XGUI_RubberBand.h
	XGUI_Constants.h
	XGUI_ViewBackground.h
	XGUI_DocumentDataModel.h
	XGUI_PartDataModel.h
	XGUI_ObjectsBrowser.h
	XGUI_OperationMgr.h
    XGUI_DataTreeModel.h
    XGUI_SelectionMgr.h
    XGUI_SalomeConnector.h
    XGUI_ActionsMgr.h
    XGUI_ErrorDialog.h
    XGUI_SalomeViewer.h
    XGUI_ViewerProxy.h
    XGUI_PropertyPanel.h
    XGUI_ContextMenuMgr.h
    XGUI_ModuleConnector.h
	XGUI_Selection.h
)

SET(PROJECT_AUTOMOC 
    ${CMAKE_CURRENT_BINARY_DIR}/XGUI_automoc.cpp
)

SET(PROJECT_SOURCES
	XGUI_Command.cpp
	XGUI_Displayer.cpp
	XGUI_MainMenu.cpp
	XGUI_MainWindow.cpp
	XGUI_MenuGroupPanel.cpp
	XGUI_Tools.cpp
	XGUI_Workbench.cpp
	XGUI_Workshop.cpp
    XGUI_ViewWindow.cpp
    XGUI_ViewPort.cpp
    XGUI_Viewer.cpp
	XGUI_RubberBand.cpp
	XGUI_ViewBackground.cpp
	XGUI_DocumentDataModel.cpp
	XGUI_PartDataModel.cpp
	XGUI_ObjectsBrowser.cpp
	XGUI_OperationMgr.cpp
    XGUI_SelectionMgr.cpp
    XGUI_ActionsMgr.cpp
    XGUI_ErrorDialog.cpp
    XGUI_ViewerProxy.cpp
    XGUI_PropertyPanel.cpp
    XGUI_ContextMenuMgr.cpp
    XGUI_ModuleConnector.cpp
	XGUI_Selection.cpp
)

SET(PROJECT_RESOURCES 
    XGUI_pictures.qrc
)

SET(TEXT_RESOURCES
	XGUI_msg_fr.ts
)

SET(PROJECT_LIBRARIES
    Events
    Config
	${QT_LIBRARIES}
	${CAS_VIEWER}
	${CAS_KERNEL}
	PyConsole
	PyInterp
	PyEvent
	ModelAPI
	ModuleBase
)

IF(WIN32) 
    SET(PROJECT_LIBRARIES ${PROJECT_LIBRARIES} opengl32)
ELSE()
    SET(PROJECT_LIBRARIES ${PROJECT_LIBRARIES} GL)
ENDIF()

QT4_ADD_RESOURCES(PROJECT_COMPILED_RESOURCES ${PROJECT_RESOURCES})
#QT5_ADD_TRANSLATION(QM_RESOURCES ${TEXT_RESOURCES})

QT4_CREATE_TRANSLATION(QM_RESOURCES 
					   ${PROJECT_SOURCES}
					   ${TEXT_RESOURCES}
					   OPTIONS -extensions cpp -no-recursive
					   )


SOURCE_GROUP ("Generated Files" FILES ${PROJECT_AUTOMOC} ${PROJECT_COMPILED_RESOURCES} ${QM_RESOURCES})
#SOURCE_GROUP ("Generated Files" FILES ${PROJECT_AUTOMOC} ${PROJECT_COMPILED_RESOURCES})
SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES} ${PROJECT_RESOURCES})

ADD_DEFINITIONS( -DXGUI_EXPORTS ${CAS_DEFINITIONS} )

INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/Events
					 ${PROJECT_SOURCE_DIR}/src/Config
					 ${PROJECT_SOURCE_DIR}/src/PyInterp
					 ${PROJECT_SOURCE_DIR}/src/PyConsole
					 ${PROJECT_SOURCE_DIR}/src/ModelAPI
					 ${PROJECT_SOURCE_DIR}/src/GeomAPI
				     ${PROJECT_SOURCE_DIR}/src/Model
					 ${PROJECT_SOURCE_DIR}/src/ModuleBase
					 ${PROJECT_SOURCE_DIR}/src/PartSetPlugin
					 ${CAS_INCLUDE_DIRS})

LINK_DIRECTORIES($ENV{PYTHON_LIB_DIR})

ADD_LIBRARY(XGUI SHARED 
	${PROJECT_SOURCES} 
	${PROJECT_HEADERS} 
	${PROJECT_COMPILED_RESOURCES} 
	${TEXT_RESOURCES}
	${QM_RESOURCES}
)

ADD_DEPENDENCIES(XGUI ModelAPI)

# The Qt5Widgets_LIBRARIES variable also includes QtGui and QtCore
TARGET_LINK_LIBRARIES(XGUI ${PROJECT_LIBRARIES})

INSTALL(TARGETS XGUI DESTINATION bin)
INSTALL(FILES ${QM_RESOURCES} DESTINATION bin)
