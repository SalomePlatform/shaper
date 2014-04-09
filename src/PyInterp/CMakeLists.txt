
SET(CMAKE_AUTOMOC ON)

# header files 
SET(PROJECT_HEADERS
  PyInterp.h
  PyInterp_Dispatcher.h
  PyInterp_Event.h
  PyInterp_Interp.h
  PyInterp_Request.h
  PyInterp_Watcher.h
)

SET(PROJECT_AUTOMOC 
    ${CMAKE_CURRENT_BINARY_DIR}/PyInterp_automoc.cpp
)

# sources / static
SET(PROJECT_SOURCES
  PyInterp_Dispatcher.cxx
  PyInterp_Event.cxx
  PyInterp_Interp.cxx
  PyInterp_Request.cxx
)

SET(PROJECT_LIBRARIES
	PyEvent
	${QT_LIBRARIES}
)

SOURCE_GROUP ("Generated Files" FILES ${PROJECT_AUTOMOC})

ADD_DEFINITIONS(-DPYINTERP_EXPORTS -DHAVE_DEBUG_PYTHON)

INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/src/PyEvent
)

ADD_LIBRARY(PyInterp STATIC 	
	${PROJECT_SOURCES} 
	${PROJECT_HEADERS} 
)

TARGET_LINK_LIBRARIES(PyInterp ${PROJECT_LIBRARIES})

#INSTALL(TARGETS PyInterp DESTINATION bin)
