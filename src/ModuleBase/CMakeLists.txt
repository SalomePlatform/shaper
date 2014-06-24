INCLUDE(Common)
SET(CMAKE_AUTOMOC ON)

SET(PROJECT_HEADERS
	ModuleBase.h
	ModuleBase_IOperation.h
	ModuleBase_Operation.h
	ModuleBase_OperationDescription.h	
	ModuleBase_ModelWidget.h
	ModuleBase_WidgetBoolValue.h
	ModuleBase_WidgetDoubleValue.h
	ModuleBase_WidgetEditor.h
	ModuleBase_WidgetFactory.h
	ModuleBase_WidgetFeature.h
	ModuleBase_WidgetPoint2D.h
	ModuleBase_WidgetSwitch.h
	ModuleBase_WidgetSelector.h
	ModuleBase_IWorkshop.h
	ModuleBase_WidgetPoint2dDistance.h
)

SET(PROJECT_SOURCES
	ModuleBase_IOperation.cpp
	ModuleBase_Operation.cpp
	ModuleBase_OperationDescription.cpp
	ModuleBase_ModelWidget.cpp
	ModuleBase_WidgetBoolValue.cpp
	ModuleBase_WidgetDoubleValue.cpp
	ModuleBase_WidgetEditor.cpp
	ModuleBase_WidgetFactory.cpp
	ModuleBase_WidgetFeature.cpp
	ModuleBase_WidgetPoint2D.cpp
	ModuleBase_WidgetSwitch.cpp
	ModuleBase_WidgetSelector.cpp
	ModuleBase_WidgetPoint2dDistance.cpp
)

SET(PROJECT_LIBRARIES
    Config
    ModelAPI
	GeomAPI
    ${QT_LIBRARIES}
	${CAS_VIEWER}
	${CAS_KERNEL}
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
)

ADD_DEFINITIONS(-DMODULEBASE_EXPORTS ${CAS_DEFINITIONS})
ADD_LIBRARY(ModuleBase SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(ModuleBase GeomAPI ${PROJECT_LIBRARIES})

INSTALL(TARGETS ModuleBase DESTINATION bin)
