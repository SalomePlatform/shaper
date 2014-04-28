INCLUDE(Common)
INCLUDE(FindCAS)

SET(CMAKE_AUTOMOC ON)

SET(PROJECT_HEADERS
	PartSet.h
	PartSet_Listener.h
	PartSet_Module.h
	PartSet_OperationSketchBase.h
	PartSet_OperationSketch.h
	PartSet_OperationSketchLine.h
	PartSet_Tools.h
)

SET(PROJECT_SOURCES
	PartSet_Listener.cpp
	PartSet_Module.cpp
	PartSet_OperationSketchBase.cpp
	PartSet_OperationSketch.cpp
	PartSet_OperationSketchLine.cpp
	PartSet_Tools.cpp
)

SET(PROJECT_RESOURCES 
    PartSet_icons.qrc
)

SET(TEXT_RESOURCES
	PartSet_msg_en.ts
)

SET(PROJECT_LIBRARIES
    ModuleBase
    Config
    GeomAPI
    ${QT_LIBRARIES}
    ${CAS_KERNEL}
)

SET(PROJECT_AUTOMOC 
    ${CMAKE_CURRENT_BINARY_DIR}/PartSet_automoc.cpp
)

QT4_ADD_RESOURCES(PROJECT_COMPILED_RESOURCES ${PROJECT_RESOURCES})
QT4_ADD_TRANSLATION(QM_RESOURCES ${TEXT_RESOURCES})

SOURCE_GROUP ("Generated Files" FILES ${PROJECT_AUTOMOC} ${PROJECT_COMPILED_RESOURCES} ${QM_RESOURCES})
SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES} ${PROJECT_RESOURCES})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/XGUI 
                    ${CMAKE_SOURCE_DIR}/src/Config
                    ${CMAKE_SOURCE_DIR}/src/Events
                    ${CMAKE_SOURCE_DIR}/src/Model
                    ${CMAKE_SOURCE_DIR}/src/ModuleBase
                    ${CMAKE_SOURCE_DIR}/src/ModelAPI
                    ${CMAKE_SOURCE_DIR}/src/GeomDataAPI
                    ${CMAKE_SOURCE_DIR}/src/GeomAlgoAPI
                    ${CMAKE_SOURCE_DIR}/src/SketchPlugin
                    ${CMAKE_SOURCE_DIR}/src/GeomAPI
                    ${CAS_INCLUDE_DIRS}
)

ADD_DEFINITIONS(-DPARTSET_EXPORTS ${CAS_DEFINITIONS})
ADD_LIBRARY(PartSet SHARED 
	${PROJECT_SOURCES} 
	${PROJECT_HEADERS} 
	${PROJECT_COMPILED_RESOURCES} 
	${TEXT_RESOURCES} 
	${QM_RESOURCES}
)

# The Qt5Widgets_LIBRARIES variable also includes QtGui and QtCore
TARGET_LINK_LIBRARIES(PartSet ${PROJECT_LIBRARIES} XGUI ModelAPI GeomAlgoAPI)

ADD_DEPENDENCIES(PartSet ModuleBase)

INSTALL(TARGETS PartSet DESTINATION bin)
