
SET(CMAKE_AUTOMOC ON)

# header files 
SET(PROJECT_HEADERS
  PyConsole.h
  PyConsole_Console.h
  PyConsole_Editor.h
  PyConsole_EnhEditor.h
  PyConsole_EnhInterp.h
  PyConsole_Event.h
  PyConsole_Interp.h
  PyConsole_Request.h
)

SET(PROJECT_AUTOMOC 
    ${CMAKE_CURRENT_BINARY_DIR}/PyConsole_automoc.cpp
)

# resource files / to be processed by lrelease
SET(TEXT_RESOURCES
  resources/PyConsole_msg_en.ts
  resources/PyConsole_msg_fr.ts
  resources/PyConsole_msg_ja.ts
)
# sources / static
SET(PROJECT_SOURCES
  PyConsole_Console.cpp
  PyConsole_Editor.cpp
  PyConsole_EnhEditor.cpp
  PyConsole_EnhInterp.cpp
  PyConsole_Event.cpp
  PyConsole_Interp.cpp
  PyConsole_Request.cpp
)

SET(PROJECT_LIBRARIES
	PyInterp
	${QT_LIBRARIES}
	${PYTHON_LIBRARIES}
)

QT4_ADD_TRANSLATION(QM_RESOURCES ${TEXT_RESOURCES})

SOURCE_GROUP ("Generated Files" FILES ${PROJECT_AUTOMOC} ${QM_RESOURCES})

INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/src/PyEvent
	${PROJECT_SOURCE_DIR}/src/PyInterp
)

ADD_DEFINITIONS(-DPYCONSOLE_EXPORTS -DHAVE_DEBUG_PYTHON)

ADD_LIBRARY(PyConsole SHARED
	${PROJECT_HEADERS}
	${PROJECT_SOURCES}
	${TEXT_RESOURCES} 
	${QM_RESOURCES}
)

TARGET_LINK_LIBRARIES(PyConsole ${PROJECT_LIBRARIES})

INSTALL(TARGETS PyConsole DESTINATION bin)


