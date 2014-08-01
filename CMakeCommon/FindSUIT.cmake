
SET(SUIT_DIR $ENV{SUIT_DIR} CACHE PATH "Path to the SUIT libraries")
IF(EXISTS ${SUIT_DIR})
	MESSAGE("-- SUIT found at " ${SUIT_DIR})

	FIND_LIBRARY(Event Event ${SUIT_DIR}/lib)
	FIND_LIBRARY(PyConsole PyConsole ${SUIT_DIR}/lib)
	FIND_LIBRARY(PyInterp PyInterp ${SUIT_DIR}/lib)
	FIND_LIBRARY(qtx qtx ${SUIT_DIR}/lib)
	FIND_LIBRARY(suit suit ${SUIT_DIR}/lib)

	SET(SUIT_INCLUDE ${SUIT_DIR}/include)
	
ENDIF(EXISTS ${SUIT_DIR})
