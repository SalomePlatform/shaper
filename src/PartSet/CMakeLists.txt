## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

INCLUDE(Common)

SET(CMAKE_AUTOMOC ON)

SET(PROJECT_HEADERS
	PartSet.h
	PartSet_Constants.h
	PartSet_CustomPrs.h
	PartSet_ExternalObjectsMgr.h
	PartSet_Module.h
	PartSet_OperationPrs.h
	PartSet_Tools.h
	PartSet_WidgetSketchLabel.h
	PartSet_Validators.h
	PartSet_WidgetPoint2d.h
	PartSet_WidgetPoint2dAngle.h
	PartSet_WidgetEditor.h
	PartSet_WidgetMultiSelector.h
	PartSet_WidgetPoint2dDistance.h
	PartSet_WidgetShapeSelector.h
	PartSet_WidgetFileSelector.h
	PartSet_Filters.h
	PartSet_LockApplyMgr.h
	PartSet_FilterInfinite.h
	PartSet_SketcherMgr.h
	PartSet_MenuMgr.h
	PartSet_DocumentDataModel.h
	PartSet_PartDataModel.h
	PartSet_DataTreeModel.h
	PartSet_WidgetSketchCreator.h
)

SET(PROJECT_SOURCES
	PartSet_CustomPrs.cpp
	PartSet_ExternalObjectsMgr.cpp
	PartSet_Module.cpp
	PartSet_OperationPrs.cpp
	PartSet_Tools.cpp
	PartSet_WidgetSketchLabel.cpp
	PartSet_Validators.cpp
	PartSet_WidgetEditor.cpp
	PartSet_WidgetMultiSelector.cpp
	PartSet_WidgetPoint2d.cpp
	PartSet_WidgetPoint2dAngle.cpp
	PartSet_WidgetPoint2dDistance.cpp
	PartSet_WidgetShapeSelector.cpp
	PartSet_WidgetFileSelector.cpp
	PartSet_Filters.cpp
	PartSet_LockApplyMgr.cpp
	PartSet_FilterInfinite.cpp
	PartSet_SketcherMgr.cpp
	PartSet_MenuMgr.cpp
	PartSet_DocumentDataModel.cpp
	PartSet_PartDataModel.cpp
	PartSet_WidgetSketchCreator.cpp
)

SET(PROJECT_RESOURCES 
    PartSet_icons.qrc
)

#SET(TEXT_RESOURCES
#	PartSet_msg_fr.ts
#)

SET(PROJECT_LIBRARIES
    ModuleBase
    Config
    GeomAPI
    GeomValidators
    GeomDataAPI
	SketcherPrs
    ${QT_LIBRARIES}
    ${CAS_KERNEL}
    ${CAS_SHAPE}
)

SET(PROJECT_AUTOMOC 
    ${CMAKE_CURRENT_BINARY_DIR}/PartSet_automoc.cpp
)

QT4_ADD_RESOURCES(PROJECT_COMPILED_RESOURCES ${PROJECT_RESOURCES})
#QT4_ADD_TRANSLATION(QM_RESOURCES ${TEXT_RESOURCES})
#QT4_CREATE_TRANSLATION(QM_RESOURCES 
#					   ${PROJECT_SOURCES}
#					   ${TEXT_RESOURCES}
#					   OPTIONS -extensions cpp -no-recursive
#					   )

#SOURCE_GROUP ("Generated Files" FILES ${PROJECT_AUTOMOC} ${PROJECT_COMPILED_RESOURCES} ${QM_RESOURCES})
SOURCE_GROUP ("Generated Files" FILES ${PROJECT_AUTOMOC} ${PROJECT_COMPILED_RESOURCES})
#SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES} ${PROJECT_RESOURCES})
SOURCE_GROUP ("Resource Files" FILES ${PROJECT_RESOURCES})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/XGUI 
                    ${CMAKE_SOURCE_DIR}/src/Config
                    ${CMAKE_SOURCE_DIR}/src/Events
                    ${CMAKE_SOURCE_DIR}/src/ModuleBase
                    ${CMAKE_SOURCE_DIR}/src/ModelAPI
                    ${CMAKE_SOURCE_DIR}/src/GeomDataAPI
                    ${CMAKE_SOURCE_DIR}/src/GeomAlgoAPI
                    ${CMAKE_SOURCE_DIR}/src/SketchPlugin
                    ${CMAKE_SOURCE_DIR}/src/SketcherPrs
                    ${CMAKE_SOURCE_DIR}/src/FeaturesPlugin
                    ${CMAKE_SOURCE_DIR}/src/PartSetPlugin
                    ${CMAKE_SOURCE_DIR}/src/GeomAPI
                    ${CMAKE_SOURCE_DIR}/src/GeomValidators
                    ${CMAKE_SOURCE_DIR}/src/AppElements
                    ${CAS_INCLUDE_DIRS}
					${SUIT_INCLUDE}
)

ADD_DEFINITIONS(-DPARTSET_EXPORTS ${CAS_DEFINITIONS})
ADD_LIBRARY(PartSet SHARED 
	${PROJECT_SOURCES} 
	${PROJECT_HEADERS} 
	${PROJECT_COMPILED_RESOURCES} 
#	${TEXT_RESOURCES} 
#	${QM_RESOURCES}
)

# The Qt5Widgets_LIBRARIES variable also includes QtGui and QtCore
TARGET_LINK_LIBRARIES(PartSet ${PROJECT_LIBRARIES} XGUI ModelAPI GeomAlgoAPI)

ADD_DEPENDENCIES(PartSet ModuleBase)

INSTALL(TARGETS PartSet DESTINATION bin)
#INSTALL(FILES ${QM_RESOURCES} DESTINATION bin)
