CMAKE_MINIMUM_REQUIRED(VERSION 2.8.10)

INCLUDE(FindCAS)

SET(CMAKE_AUTOMOC ON)

SET(PROJECT_HEADERS
	XGUI_Command.h
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
	XGUI_WidgetFactory.h
	XGUI_DocumentDataModel.h
	XGUI_PartDataModel.h
	XGUI_ObjectsBrowser.h
    XGUI_DataTreeModel.h
    XGUI_SelectionMgr.h
)

SET(PROJECT_AUTOMOC 
    ${CMAKE_CURRENT_BINARY_DIR}/XGUI_automoc.cpp
)

SET(PROJECT_SOURCES
	XGUI_Command.cpp
	XGUI_Main.cpp
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
	XGUI_WidgetFactory.cpp
	XGUI_DocumentDataModel.cpp
	XGUI_PartDataModel.cpp
	XGUI_ObjectsBrowser.cpp
    XGUI_SelectionMgr.cpp
)

SET(PROJECT_RESOURCES 
    XGUI_pictures.qrc
)

#SET(TEXT_RESOURCES
#	XGUI_msg_en.ts
#)

SET(PROJECT_LIBRARIES
	opengl32
    Event
    Config
	${QT_LIBRARIES}
	${QT_QTMAIN_LIBRARY}
	${CAS_VIEWER}
	${CAS_KERNEL}
	PyConsole
	PyInterp
	PyEvent
	ModelAPI
	ModuleBase
)

QT4_ADD_RESOURCES(PROJECT_COMPILED_RESOURCES ${PROJECT_RESOURCES})
#QT5_ADD_TRANSLATION(QM_RESOURCES ${TEXT_RESOURCES})

#SOURCE_GROUP ("Generated Files" FILES ${PROJECT_AUTOMOC} ${PROJECT_COMPILED_RESOURCES} ${QM_RESOURCES})
SOURCE_GROUP ("Generated Files" FILES ${PROJECT_AUTOMOC} ${PROJECT_COMPILED_RESOURCES})
SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES} ${PROJECT_RESOURCES})

ADD_DEFINITIONS(${CAS_DEFINITIONS} )

INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/Event
					 ${PROJECT_SOURCE_DIR}/src/Config
					 ${PROJECT_SOURCE_DIR}/src/PyInterp
					 ${PROJECT_SOURCE_DIR}/src/PyConsole
					 ${PROJECT_SOURCE_DIR}/src/ModelAPI
					 ${PROJECT_SOURCE_DIR}/src/Model
					 ${PROJECT_SOURCE_DIR}/src/ModuleBase
					 ${CAS_INCLUDE_DIRS})

LINK_DIRECTORIES($ENV{PYTHON_LIB_DIR})

ADD_EXECUTABLE(XGUI WIN32 
	${PROJECT_SOURCES} 
	${PROJECT_HEADERS} 
	${PROJECT_COMPILED_RESOURCES} 
)

ADD_DEPENDENCIES(XGUI ModelAPI)

# The Qt5Widgets_LIBRARIES variable also includes QtGui and QtCore
TARGET_LINK_LIBRARIES(XGUI ${PROJECT_LIBRARIES})

INSTALL(TARGETS XGUI DESTINATION bin)
#INSTALL(FILES ${QM_RESOURCES} DESTINATION bin)
