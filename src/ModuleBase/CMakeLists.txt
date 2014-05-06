INCLUDE(Common)
SET(CMAKE_AUTOMOC ON)

SET(PROJECT_HEADERS
    ModuleBase.h
	ModuleBase_IOperation.h
	ModuleBase_Operation.h
	ModuleBase_OperationDescription.h	
	ModuleBase_ModelWidget.h
	ModuleBase_WidgetFactory.h
	ModuleBase_WidgetPoint2D.h
	ModuleBase_WidgetSwitch.h
	ModuleBase_MetaWidget.h
)

SET(PROJECT_SOURCES
	ModuleBase_IOperation.cpp
	ModuleBase_Operation.cpp
	ModuleBase_OperationDescription.cpp
	ModuleBase_WidgetFactory.cpp
	ModuleBase_WidgetPoint2D.cpp
	ModuleBase_WidgetSwitch.cpp
	ModuleBase_MetaWidget.cpp
)

SET(PROJECT_LIBRARIES
    Config
    ModelAPI
    ${QT_LIBRARIES}
)

SET(PROJECT_AUTOMOC 
    ${CMAKE_CURRENT_BINARY_DIR}/ModuleBase_automoc.cpp
)

#QT5_ADD_RESOURCES(PROJECT_COMPILED_RESOURCES ${PROJECT_RESOURCES})
#QT5_ADD_TRANSLATION(QM_RESOURCES ${TEXT_RESOURCES})

SOURCE_GROUP ("Generated Files" FILES ${PROJECT_AUTOMOC} ${PROJECT_COMPILED_RESOURCES} ${QM_RESOURCES})
#SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES} ${PROJECT_RESOURCES})

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/src/Config
    ${CMAKE_SOURCE_DIR}/src/ModelAPI
    ${CMAKE_SOURCE_DIR}/src/GeomDataAPI
)

ADD_DEFINITIONS(-DMODULEBASE_EXPORTS)
ADD_LIBRARY(ModuleBase SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(ModuleBase ${PROJECT_LIBRARIES})

INSTALL(TARGETS ModuleBase DESTINATION bin)
