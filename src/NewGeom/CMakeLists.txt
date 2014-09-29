
SET(CMAKE_AUTOMOC ON)

SET(PROJECT_HEADERS
	NewGeom.h
	NewGeom_Module.h
	NewGeom_DataModel.h
	NewGeom_OCCSelector.h
	NewGeom_SalomeViewer.h
)

SET(PROJECT_AUTOMOC 
    ${CMAKE_CURRENT_BINARY_DIR}/NewGeom_automoc.cpp
)

SET(PROJECT_SOURCES
	NewGeom_Module.cpp
	NewGeom_DataModel.cpp
	NewGeom_OCCSelector.cpp
	NewGeom_SalomeViewer.cpp
)

SET(PROJECT_RESOURCES
	resources/LightApp.xml
	resources/SalomeApp.xml
	resources/newgeom.png
)

SET(PROJECT_LIBRARIES
    Events
    Config
	XGUI
	${QT_LIBRARIES}
	${suit}
	${std}
	${LightApp}
	${CAM}
	${OCCViewer}
	${qtx}
	${CAS_SHAPE}
)

ADD_DEFINITIONS( -DNewGeom_EXPORTS ${CAS_DEFINITIONS} )

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/XGUI
					${PROJECT_SOURCE_DIR}/src/Events
					${PROJECT_SOURCE_DIR}/src/ModuleBase
                    ${PROJECT_SOURCE_DIR}/src/ModelAPI
                    ${PROJECT_SOURCE_DIR}/src/Config
					${SALOME_GUI_INCLUDE}
					${SALOME_KERNEL_INCLUDE}
                    ${CAS_INCLUDE_DIRS}
)

ADD_LIBRARY(NewGeom SHARED 
	${PROJECT_SOURCES} 
	${PROJECT_HEADERS} 
)

ADD_DEPENDENCIES(NewGeom XGUI)

# The Qt5Widgets_LIBRARIES variable also includes QtGui and QtCore
TARGET_LINK_LIBRARIES(NewGeom ${PROJECT_LIBRARIES})

INSTALL(TARGETS NewGeom DESTINATION bin)
INSTALL(FILES ${PROJECT_RESOURCES} DESTINATION share/salome/resources/newgeom )
