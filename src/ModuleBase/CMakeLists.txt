## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

INCLUDE(Common)
SET(CMAKE_AUTOMOC ON)

SET(PROJECT_HEADERS
	ModuleBase.h
	ModuleBase_Filter.h
	ModuleBase_FilterCustom.h
	ModuleBase_FilterFace.h
	ModuleBase_FilterFactory.h
	ModuleBase_FilterLinearEdge.h
	ModuleBase_FilterMulti.h
	ModuleBase_FilterNoConsructionSubShapes.h
	ModuleBase_FilterNoDegeneratedEdge.h
	ModuleBase_FilterShapeType.h
	ModuleBase_FilterValidated.h
	ModuleBase_Tools.h
	ModuleBase_IModule.h
	ModuleBase_Operation.h
	ModuleBase_OperationDescription.h	
	ModuleBase_ModelWidget.h
	ModuleBase_WidgetBoolValue.h
	ModuleBase_WidgetDoubleValue.h
	ModuleBase_WidgetEditor.h
	ModuleBase_WidgetFactory.h
	ModuleBase_WidgetShapeSelector.h
	ModuleBase_IWorkshop.h
	ModuleBase_Definitions.h
	ModuleBase_SelectionValidator.h
	ModuleBase_ISelection.h
	ModuleBase_ViewerPrs.h
	ModuleBase_WidgetChoice.h
	ModuleBase_WidgetFileSelector.h
	ModuleBase_DoubleSpinBox.h
	ModuleBase_IPropertyPanel.h
	ModuleBase_IViewer.h
	ModuleBase_WidgetLineEdit.h
	ModuleBase_WidgetMultiSelector.h
	ModuleBase_ViewerFilters.h
	ModuleBase_ResultPrs.h
	ModuleBase_IViewWindow.h
	ModuleBase_WidgetLabel.h
	ModuleBase_IPrefMgr.h
	ModuleBase_Preferences.h
	ModuleBase_ActionInfo.h
	ModuleBase_PageBase.h
	ModuleBase_PageWidget.h	
	ModuleBase_PageGroupBox.h
	ModuleBase_PagedContainer.h
	ModuleBase_WidgetSwitch.h
	ModuleBase_WidgetToolbox.h
	ModuleBase_WidgetValidated.h
)

SET(PROJECT_SOURCES
	ModuleBase_Filter.cpp
	ModuleBase_FilterCustom.cpp
	ModuleBase_FilterFace.cpp
	ModuleBase_FilterFactory.cpp
	ModuleBase_FilterLinearEdge.cpp
	ModuleBase_FilterMulti.cpp
	ModuleBase_FilterNoConsructionSubShapes.cpp
	ModuleBase_FilterNoDegeneratedEdge.cpp
	ModuleBase_FilterShapeType.cpp
	ModuleBase_FilterValidated.cpp
	ModuleBase_Tools.cpp
	ModuleBase_IModule.cpp
	ModuleBase_IWorkshop.cpp
	ModuleBase_Operation.cpp
	ModuleBase_OperationDescription.cpp
	ModuleBase_ModelWidget.cpp
	ModuleBase_WidgetBoolValue.cpp
	ModuleBase_WidgetDoubleValue.cpp
	ModuleBase_WidgetEditor.cpp
	ModuleBase_WidgetFactory.cpp
	ModuleBase_WidgetShapeSelector.cpp
	ModuleBase_WidgetChoice.cpp
	ModuleBase_WidgetFileSelector.cpp
	ModuleBase_DoubleSpinBox.cpp
	ModuleBase_WidgetLineEdit.cpp
	ModuleBase_WidgetMultiSelector.cpp
	ModuleBase_ViewerFilters.cpp
	ModuleBase_ResultPrs.cpp
	ModuleBase_WidgetLabel.cpp
	ModuleBase_Preferences.cpp
	ModuleBase_ActionInfo.cpp
	ModuleBase_PageBase.cpp
	ModuleBase_PageWidget.cpp
	ModuleBase_PageGroupBox.cpp
	ModuleBase_PagedContainer.cpp
	ModuleBase_WidgetSwitch.cpp
	ModuleBase_WidgetToolbox.cpp
	ModuleBase_WidgetValidated.cpp
)

SET(PROJECT_LIBRARIES
	Config
	Events
	ModelAPI
	GeomAPI
	GeomAlgoAPI
	${QT_LIBRARIES}
	${CAS_VIEWER}
	${CAS_KERNEL}
	${CAS_SHAPE}
	${suit}
	${qtx}
)

SET(PROJECT_AUTOMOC 
    ${CMAKE_CURRENT_BINARY_DIR}/ModuleBase_automoc.cpp
)

#QT5_ADD_RESOURCES(PROJECT_COMPILED_RESOURCES ${PROJECT_RESOURCES})
#QT5_ADD_TRANSLATION(QM_RESOURCES ${TEXT_RESOURCES})

SOURCE_GROUP ("Generated Files" FILES ${PROJECT_AUTOMOC} ${PROJECT_COMPILED_RESOURCES} ${QM_RESOURCES})
#SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES} ${PROJECT_RESOURCES})

INCLUDE_DIRECTORIES(
    ${CAS_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src/Config
    ${CMAKE_SOURCE_DIR}/src/Events
    ${CMAKE_SOURCE_DIR}/src/Model
    ${CMAKE_SOURCE_DIR}/src/ModelAPI
    ${CMAKE_SOURCE_DIR}/src/GeomDataAPI
    ${CMAKE_SOURCE_DIR}/src/GeomAPI
    ${CMAKE_SOURCE_DIR}/src/GeomAlgoAPI
    ${SUIT_INCLUDE}
)

ADD_DEFINITIONS(-DMODULEBASE_EXPORTS ${CAS_DEFINITIONS})
ADD_LIBRARY(ModuleBase SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(ModuleBase GeomAPI ${PROJECT_LIBRARIES})

INSTALL(TARGETS ModuleBase DESTINATION bin)
